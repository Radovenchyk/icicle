package internal

import (
	"testing"

	"github.com/stretchr/testify/assert"
)

func TestMockAffineZero(t *testing.T) {
	var fieldZero = MockField{}

	var affineZero MockAffine
	assert.Equal(t, affineZero.X, fieldZero)
	assert.Equal(t, affineZero.Y, fieldZero)

	x := generateRandomLimb(int(BASE_LIMBS))
	y := generateRandomLimb(int(BASE_LIMBS))
	var affine MockAffine
	affine.FromLimbs(x, y)

	affine.Zero()
	assert.Equal(t, affine.X, fieldZero)
	assert.Equal(t, affine.Y, fieldZero)
}

func TestMockAffineFromLimbs(t *testing.T) {
	randLimbs := generateRandomLimb(int(BASE_LIMBS))
	randLimbs2 := generateRandomLimb(int(BASE_LIMBS))

	var affine MockAffine
	affine.FromLimbs(randLimbs, randLimbs2)

	assert.ElementsMatch(t, randLimbs, affine.X.GetLimbs())
	assert.ElementsMatch(t, randLimbs2, affine.Y.GetLimbs())
}

func TestMockAffineToProjective(t *testing.T) {
	randLimbs := generateRandomLimb(int(BASE_LIMBS))
	randLimbs2 := generateRandomLimb(int(BASE_LIMBS))
	var fieldOne MockField
	fieldOne.One()

	var expected MockProjective
	expected.FromLimbs(randLimbs, randLimbs2, fieldOne.limbs[:])

	var affine MockAffine
	affine.FromLimbs(randLimbs, randLimbs2)

	projectivePoint := affine.ToProjective()
	assert.Equal(t, expected, projectivePoint)
}

func TestMockProjectiveZero(t *testing.T) {
	var projectiveZero MockProjective
	projectiveZero.Zero()
	var fieldZero = MockField{}
	var fieldOne MockField
	fieldOne.One()

	assert.Equal(t, projectiveZero.X, fieldZero)
	assert.Equal(t, projectiveZero.Y, fieldOne)
	assert.Equal(t, projectiveZero.Z, fieldZero)

	randLimbs := generateRandomLimb(int(BASE_LIMBS))
	var projective MockProjective
	projective.FromLimbs(randLimbs, randLimbs, randLimbs)

	projective.Zero()
	assert.Equal(t, projective.X, fieldZero)
	assert.Equal(t, projective.Y, fieldOne)
	assert.Equal(t, projective.Z, fieldZero)
}

func TestMockProjectiveFromLimbs(t *testing.T) {
	randLimbs := generateRandomLimb(int(BASE_LIMBS))
	randLimbs2 := generateRandomLimb(int(BASE_LIMBS))
	randLimbs3 := generateRandomLimb(int(BASE_LIMBS))

	var projective MockProjective
	projective.FromLimbs(randLimbs, randLimbs2, randLimbs3)

	assert.ElementsMatch(t, randLimbs, projective.X.GetLimbs())
	assert.ElementsMatch(t, randLimbs2, projective.Y.GetLimbs())
	assert.ElementsMatch(t, randLimbs3, projective.Z.GetLimbs())
}

func TestMockProjectiveFromAffine(t *testing.T) {
	randLimbs := generateRandomLimb(int(BASE_LIMBS))
	randLimbs2 := generateRandomLimb(int(BASE_LIMBS))
	var fieldOne MockField
	fieldOne.One()

	var expected MockProjective
	expected.FromLimbs(randLimbs, randLimbs2, fieldOne.limbs[:])

	var affine MockAffine
	affine.FromLimbs(randLimbs, randLimbs2)

	var projectivePoint MockProjective
	projectivePoint.FromAffine(affine)
	assert.Equal(t, expected, projectivePoint)
}