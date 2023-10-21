// Copyright 2023 Ingonyama
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Code generated by Ingonyama DO NOT EDIT

package bw6761

import (
	"fmt"
	"math"
	"testing"
	"time"
	"unsafe"

	"github.com/ingonyama-zk/icicle/goicicle"
	"github.com/stretchr/testify/assert"
)

func GeneratePoints(count int) []G1PointAffine {
	// Declare a slice of integers
	var points []G1PointAffine

	// populate the slice
	for i := 0; i < 10; i++ {
		var pointProjective G1ProjectivePoint
		pointProjective.Random()

		var pointAffine G1PointAffine
		pointAffine.FromProjective(&pointProjective)

		points = append(points, pointAffine)
	}

	log2_10 := math.Log2(10)
	log2Count := math.Log2(float64(count))
	log2Size := int(math.Ceil(log2Count - log2_10))

	for i := 0; i < log2Size; i++ {
		points = append(points, points...)
	}

	return points[:count]
}

func GeneratePointsProj(count int) []G1ProjectivePoint {
	// Declare a slice of integers
	var points []G1ProjectivePoint
	// Use a loop to populate the slice
	for i := 0; i < count; i++ {
		var p G1ProjectivePoint
		p.Random()

		points = append(points, p)
	}

	return points
}

func GenerateScalars(count int, skewed bool) []G1ScalarField {
	// Declare a slice of integers
	var scalars []G1ScalarField

	var rand G1ScalarField
	var zero G1ScalarField
	var one G1ScalarField
	var randLarge G1ScalarField

	zero.SetZero()
	one.SetOne()
	randLarge.Random()

	if skewed && count > 1_200_000 {
		for i := 0; i < count-1_200_000; i++ {
			rand.Random()
			scalars = append(scalars, rand)
		}

		for i := 0; i < 600_000; i++ {
			scalars = append(scalars, randLarge)
		}
		for i := 0; i < 400_000; i++ {
			scalars = append(scalars, zero)
		}
		for i := 0; i < 200_000; i++ {
			scalars = append(scalars, one)
		}
	} else {
		for i := 0; i < count; i++ {
			rand.Random()
			scalars = append(scalars, rand)
		}
	}

	return scalars[:count]
}

func TestMSM(t *testing.T) {
	fmt.Print() // this prevents the test from hanging. TODO: figure out why
	for _, v := range []int{8} {
		count := 1 << v

		points := GeneratePoints(count)
		fmt.Print("Finished generating points\n")
		scalars := GenerateScalars(count, false)
		fmt.Print("Finished generating scalars\n")

		out := new(G1ProjectivePoint)
		startTime := time.Now()
		_, e := Msm(out, points, scalars, 0) // non mont
		fmt.Printf("icicle MSM took: %d ms\n", time.Since(startTime).Milliseconds())

		assert.Equal(t, e, nil, "error should be nil")

		assert.True(t, out.IsOnCurve())
	}
}

func TestCommitMSM(t *testing.T) {
	for _, v := range []int{8} {
		count := 1<<v - 1
		fmt.Print("Started generating points and scalars\n")
		points := GeneratePoints(count)
		scalars := GenerateScalars(count, false)
		fmt.Print("Finished generating points and scalars\n")

		var sizeOutD G1ProjectivePoint
		out_d, _ := goicicle.CudaMalloc(int(unsafe.Sizeof(sizeOutD)))

		var sizePoints G1PointAffine
		pointsBytes := count * int(unsafe.Sizeof(sizePoints))
		points_d, _ := goicicle.CudaMalloc(pointsBytes)
		goicicle.CudaMemCpyHtoD[G1PointAffine](points_d, points, pointsBytes)

		var sizeScalar G1ScalarField
		scalarBytes := count * int(unsafe.Sizeof(sizeScalar))
		scalars_d, _ := goicicle.CudaMalloc(scalarBytes)
		goicicle.CudaMemCpyHtoD[G1ScalarField](scalars_d, scalars, scalarBytes)

		startTime := time.Now()
		e := Commit(out_d, scalars_d, points_d, count, 10)
		fmt.Printf("icicle MSM took: %d ms\n", time.Since(startTime).Milliseconds())

		outHost := make([]G1ProjectivePoint, 1)
		goicicle.CudaMemCpyDtoH[G1ProjectivePoint](outHost, out_d, int(unsafe.Sizeof(sizeOutD)))

		assert.Equal(t, e, 0, "error should be 0")
		assert.True(t, outHost[0].IsOnCurve())
	}
}

func BenchmarkCommit(b *testing.B) {
	LOG_MSM_SIZES := []int{20, 21, 22, 23, 24, 25, 26}

	for _, logMsmSize := range LOG_MSM_SIZES {
		msmSize := 1 << logMsmSize
		points := GeneratePoints(msmSize)
		scalars := GenerateScalars(msmSize, false)

		out_d, _ := goicicle.CudaMalloc(96)

		pointsBytes := msmSize * 64
		points_d, _ := goicicle.CudaMalloc(pointsBytes)
		goicicle.CudaMemCpyHtoD[G1PointAffine](points_d, points, pointsBytes)

		scalarBytes := msmSize * 32
		scalars_d, _ := goicicle.CudaMalloc(scalarBytes)
		goicicle.CudaMemCpyHtoD[G1ScalarField](scalars_d, scalars, scalarBytes)

		b.Run(fmt.Sprintf("MSM %d", logMsmSize), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				e := Commit(out_d, scalars_d, points_d, msmSize, 10)

				assert.Equal(b, e, 0, "error should be 0")
				outHost := make([]G1ProjectivePoint, 1)
				goicicle.CudaMemCpyDtoH[G1ProjectivePoint](outHost, out_d, 288)
				assert.True(b, outHost[0].IsOnCurve())
				if e != 0 {
					panic("Error occured")
				}
			}
		})
	}
}

func TestBatchMSM(t *testing.T) {
	for _, batchPow2 := range []int{2, 4} {
		for _, pow2 := range []int{4, 6} {
			msmSize := 1 << pow2
			batchSize := 1 << batchPow2
			count := msmSize * batchSize

			points := GeneratePoints(count)
			scalars := GenerateScalars(count, false)

			pointsResults, e := MsmBatch(&points, &scalars, batchSize, 0)

			if e != nil {
				t.Errorf("MsmBatchBW6761 returned an error: %v", e)
			}

			if len(pointsResults) != batchSize {
				t.Errorf("Expected length %d, but got %d", batchSize, len(pointsResults))
			}

			for _, s := range pointsResults {
				assert.True(t, s.IsOnCurve())
			}
		}
	}
}

func BenchmarkMSM(b *testing.B) {
	LOG_MSM_SIZES := []int{20, 21, 22, 23, 24, 25, 26}

	for _, logMsmSize := range LOG_MSM_SIZES {
		msmSize := 1 << logMsmSize
		points := GeneratePoints(msmSize)
		scalars := GenerateScalars(msmSize, false)
		b.Run(fmt.Sprintf("MSM %d", logMsmSize), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				out := new(G1ProjectivePoint)
				_, e := Msm(out, points, scalars, 0)

				if e != nil {
					panic("Error occured")
				}
			}
		})
	}
}

// G2
func GenerateG2Points(count int) []G2PointAffine {
	// Declare a slice of integers
	var points []G2PointAffine

	// populate the slice
	for i := 0; i < 10; i++ {
		fmt.Print() // this prevents the test from hanging. TODO: figure out why
		var p G2Point
		p.Random()
		var affine G2PointAffine
		affine.FromProjective(&p)

		points = append(points, affine)
	}

	log2_10 := math.Log2(10)
	log2Count := math.Log2(float64(count))
	log2Size := int(math.Ceil(log2Count - log2_10))

	for i := 0; i < log2Size; i++ {
		points = append(points, points...)
	}

	return points[:count]
}

func TestMsmG2BW6761(t *testing.T) {
	for _, v := range []int{8} {
		count := 1 << v
		points := GenerateG2Points(count)
		fmt.Print("Finished generating points\n")
		scalars := GenerateScalars(count, false)
		fmt.Print("Finished generating scalars\n")

		out := new(G2Point)
		_, e := MsmG2(out, points, scalars, 0)
		assert.Equal(t, e, nil, "error should be nil")
		assert.True(t, out.IsOnCurve())
	}
}

func BenchmarkMsmG2BW6761(b *testing.B) {
	LOG_MSM_SIZES := []int{20, 21, 22, 23, 24, 25, 26}

	for _, logMsmSize := range LOG_MSM_SIZES {
		msmSize := 1 << logMsmSize
		points := GenerateG2Points(msmSize)
		scalars := GenerateScalars(msmSize, false)
		b.Run(fmt.Sprintf("MSM G2 %d", logMsmSize), func(b *testing.B) {
			for n := 0; n < b.N; n++ {
				out := new(G2Point)
				_, e := MsmG2(out, points, scalars, 0)

				if e != nil {
					panic("Error occured")
				}
			}
		})
	}
}

func TestCommitG2MSM(t *testing.T) {
	for _, v := range []int{8} {
		count := 1 << v

		points := GenerateG2Points(count)
		fmt.Print("Finished generating points\n")
		scalars := GenerateScalars(count, false)
		fmt.Print("Finished generating scalars\n")

		var sizeCheckG2PointAffine G2PointAffine
		inputPointsBytes := count * int(unsafe.Sizeof(sizeCheckG2PointAffine))

		var sizeCheckG2Point G2Point
		out_d, _ := goicicle.CudaMalloc(int(unsafe.Sizeof(sizeCheckG2Point)))

		points_d, _ := goicicle.CudaMalloc(inputPointsBytes)
		goicicle.CudaMemCpyHtoD[G2PointAffine](points_d, points, inputPointsBytes)

		scalarBytes := count * 32
		scalars_d, _ := goicicle.CudaMalloc(scalarBytes)
		goicicle.CudaMemCpyHtoD[G1ScalarField](scalars_d, scalars, scalarBytes)

		startTime := time.Now()
		e := CommitG2(out_d, scalars_d, points_d, count, 10)
		fmt.Printf("icicle MSM took: %d ms\n", time.Since(startTime).Milliseconds())

		outHost := make([]G2Point, 1)
		goicicle.CudaMemCpyDtoH[G2Point](outHost, out_d, int(unsafe.Sizeof(sizeCheckG2Point)))

		assert.Equal(t, e, 0, "error should be 0")
		assert.Equal(t, len(outHost), 1)
		result := outHost[0]

		assert.True(t, result.IsOnCurve())
	}
}

func TestBatchG2MSM(t *testing.T) {
	for _, batchPow2 := range []int{2, 4} {
		for _, pow2 := range []int{4, 6} {
			msmSize := 1 << pow2
			batchSize := 1 << batchPow2
			count := msmSize * batchSize

			points := GenerateG2Points(count)
			scalars := GenerateScalars(count, false)

			pointsResults, e := MsmG2Batch(&points, &scalars, batchSize, 0)

			if e != nil {
				t.Errorf("MsmBatchBW6761 returned an error: %v", e)
			}

			if len(pointsResults) != batchSize {
				t.Errorf("Expected length %d, but got %d", batchSize, len(pointsResults))
			}

			for _, s := range pointsResults {
				assert.True(t, s.IsOnCurve())
			}
		}
	}
}
