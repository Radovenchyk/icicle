"use strict";(self.webpackChunkdocusaurus=self.webpackChunkdocusaurus||[]).push([[708],{5680:(e,n,a)=>{a.d(n,{xA:()=>c,yg:()=>m});var t=a(6540);function r(e,n,a){return n in e?Object.defineProperty(e,n,{value:a,enumerable:!0,configurable:!0,writable:!0}):e[n]=a,e}function i(e,n){var a=Object.keys(e);if(Object.getOwnPropertySymbols){var t=Object.getOwnPropertySymbols(e);n&&(t=t.filter((function(n){return Object.getOwnPropertyDescriptor(e,n).enumerable}))),a.push.apply(a,t)}return a}function l(e){for(var n=1;n<arguments.length;n++){var a=null!=arguments[n]?arguments[n]:{};n%2?i(Object(a),!0).forEach((function(n){r(e,n,a[n])})):Object.getOwnPropertyDescriptors?Object.defineProperties(e,Object.getOwnPropertyDescriptors(a)):i(Object(a)).forEach((function(n){Object.defineProperty(e,n,Object.getOwnPropertyDescriptor(a,n))}))}return e}function o(e,n){if(null==e)return{};var a,t,r=function(e,n){if(null==e)return{};var a,t,r={},i=Object.keys(e);for(t=0;t<i.length;t++)a=i[t],n.indexOf(a)>=0||(r[a]=e[a]);return r}(e,n);if(Object.getOwnPropertySymbols){var i=Object.getOwnPropertySymbols(e);for(t=0;t<i.length;t++)a=i[t],n.indexOf(a)>=0||Object.prototype.propertyIsEnumerable.call(e,a)&&(r[a]=e[a])}return r}var g=t.createContext({}),p=function(e){var n=t.useContext(g),a=n;return e&&(a="function"==typeof e?e(n):l(l({},n),e)),a},c=function(e){var n=p(e.components);return t.createElement(g.Provider,{value:n},e.children)},s="mdxType",d={inlineCode:"code",wrapper:function(e){var n=e.children;return t.createElement(t.Fragment,{},n)}},u=t.forwardRef((function(e,n){var a=e.components,r=e.mdxType,i=e.originalType,g=e.parentName,c=o(e,["components","mdxType","originalType","parentName"]),s=p(a),u=r,m=s["".concat(g,".").concat(u)]||s[u]||d[u]||i;return a?t.createElement(m,l(l({ref:n},c),{},{components:a})):t.createElement(m,l({ref:n},c))}));function m(e,n){var a=arguments,r=n&&n.mdxType;if("string"==typeof e||r){var i=a.length,l=new Array(i);l[0]=u;var o={};for(var g in n)hasOwnProperty.call(n,g)&&(o[g]=n[g]);o.originalType=e,o[s]="string"==typeof e?e:r,l[1]=o;for(var p=2;p<i;p++)l[p]=a[p];return t.createElement.apply(null,l)}return t.createElement.apply(null,a)}u.displayName="MDXCreateElement"},3139:(e,n,a)=>{a.r(n),a.d(n,{assets:()=>g,contentTitle:()=>l,default:()=>d,frontMatter:()=>i,metadata:()=>o,toc:()=>p});var t=a(8168),r=(a(6540),a(5680));a(1873);const i={},l="Golang bindings",o={unversionedId:"icicle/golang-bindings",id:"icicle/golang-bindings",title:"Golang bindings",description:"Golang bindings allow you to use ICICLE as a golang library.",source:"@site/docs/icicle/golang-bindings.md",sourceDirName:"icicle",slug:"/icicle/golang-bindings",permalink:"/icicle/golang-bindings",editUrl:"https://github.com/ingonyama-zk/icicle/tree/main/docs/icicle/golang-bindings.md",tags:[],version:"current",lastUpdatedBy:"Otsar",lastUpdatedAt:1725444871,formattedLastUpdatedAt:"9/4/2024",frontMatter:{},sidebar:"GettingStartedSidebar",previous:{title:"Polynomial API Overview",permalink:"/icicle/polynomials/overview"},next:{title:"MSM",permalink:"/icicle/golang-bindings/msm"}},g={},p=[{value:"Using ICICLE Golang bindings in your project",id:"using-icicle-golang-bindings-in-your-project",level:2},{value:"Building from source",id:"building-from-source",level:3},{value:"Building with precompiled libs",id:"building-with-precompiled-libs",level:3},{value:"Running tests",id:"running-tests",level:2},{value:"How do Golang bindings work?",id:"how-do-golang-bindings-work",level:2},{value:"Supported curves, fields and operations",id:"supported-curves-fields-and-operations",level:2},{value:"Supported curves and operations",id:"supported-curves-and-operations",level:3},{value:"Supported fields and operations",id:"supported-fields-and-operations",level:3}],c={toc:p},s="wrapper";function d(e){let{components:n,...a}=e;return(0,r.yg)(s,(0,t.A)({},c,a,{components:n,mdxType:"MDXLayout"}),(0,r.yg)("h1",{id:"golang-bindings"},"Golang bindings"),(0,r.yg)("p",null,"Golang bindings allow you to use ICICLE as a golang library.\nThe source code for all Golang packages can be found ",(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang"},"here"),"."),(0,r.yg)("p",null,"The Golang bindings are comprised of multiple packages."),(0,r.yg)("p",null,(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang/core"},(0,r.yg)("inlineCode",{parentName:"a"},"core"))," which defines all shared methods and structures, such as configuration structures, or memory slices."),(0,r.yg)("p",null,(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang/runtime"},(0,r.yg)("inlineCode",{parentName:"a"},"runtime"))," which defines abstractions for ICICLE methods for allocating memory, initializing and managing streams, and ",(0,r.yg)("inlineCode",{parentName:"p"},"Device")," which enables users to define and keep track of devices."),(0,r.yg)("p",null,"Each supported curve and field has its own package which you can find in the respective directories ",(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang"},"here"),". If your project uses BN254 you only need to import that single package named ",(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang/curves/bn254"},(0,r.yg)("inlineCode",{parentName:"a"},"bn254")),"."),(0,r.yg)("h2",{id:"using-icicle-golang-bindings-in-your-project"},"Using ICICLE Golang bindings in your project"),(0,r.yg)("p",null,"To add ICICLE to your ",(0,r.yg)("inlineCode",{parentName:"p"},"go.mod")," file."),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"go get github.com/ingonyama-zk/icicle/v3\n")),(0,r.yg)("p",null,"If you want to specify a specific branch"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"go get github.com/ingonyama-zk/icicle/v3@<branch_name>\n")),(0,r.yg)("p",null,"For a specific commit"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"go get github.com/ingonyama-zk/icicle/v3@<commit_id>\n")),(0,r.yg)("h3",{id:"building-from-source"},"Building from source"),(0,r.yg)("p",null,"To build the shared libraries you can run ",(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/tree/main/wrappers/golang/build.sh"},"this")," script inside the downloaded go dependency:"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-sh"},'./build.sh [-curve=<curve>] [-field=<field>] [-cuda_version=<version>] [-skip_msm] [-skip_ntt] [-skip_g2] [-skip_ecntt] [-skip_fieldext]\n\ncurve - The name of the curve to build or "all" to build all supported curves\nfield - The name of the field to build or "all" to build all supported fields\n-skip_msm - Optional - build with MSM disabled\n-skip_ntt - Optional - build with NTT disabled\n-skip_g2 - Optional - build with G2 disabled \n-skip_ecntt - Optional - build with ECNTT disabled\n-skip_fieldext - Optional - build without field extension\n-help - Optional - Displays usage information\n')),(0,r.yg)("div",{className:"admonition admonition-note alert alert--secondary"},(0,r.yg)("div",{parentName:"div",className:"admonition-heading"},(0,r.yg)("h5",{parentName:"div"},(0,r.yg)("span",{parentName:"h5",className:"admonition-icon"},(0,r.yg)("svg",{parentName:"span",xmlns:"http://www.w3.org/2000/svg",width:"14",height:"16",viewBox:"0 0 14 16"},(0,r.yg)("path",{parentName:"svg",fillRule:"evenodd",d:"M6.3 5.69a.942.942 0 0 1-.28-.7c0-.28.09-.52.28-.7.19-.18.42-.28.7-.28.28 0 .52.09.7.28.18.19.28.42.28.7 0 .28-.09.52-.28.7a1 1 0 0 1-.7.3c-.28 0-.52-.11-.7-.3zM8 7.99c-.02-.25-.11-.48-.31-.69-.2-.19-.42-.3-.69-.31H6c-.27.02-.48.13-.69.31-.2.2-.3.44-.31.69h1v3c.02.27.11.5.31.69.2.2.42.31.69.31h1c.27 0 .48-.11.69-.31.2-.19.3-.42.31-.69H8V7.98v.01zM7 2.3c-3.14 0-5.7 2.54-5.7 5.68 0 3.14 2.56 5.7 5.7 5.7s5.7-2.55 5.7-5.7c0-3.15-2.56-5.69-5.7-5.69v.01zM7 .98c3.86 0 7 3.14 7 7s-3.14 7-7 7-7-3.12-7-7 3.14-7 7-7z"}))),"note")),(0,r.yg)("div",{parentName:"div",className:"admonition-content"},(0,r.yg)("p",{parentName:"div"},"If more than one curve or more than one field is supplied, the last one supplied will be built"))),(0,r.yg)("p",null,"To build ICICLE libraries for all supported curves without certain features, you can use their -skip_<feature",">"," flags. For example, for disabling G2 and ECNTT:"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"./build.sh -curve=all -skip_g2 -skip_ecntt\n")),(0,r.yg)("p",null,"By default, all features are enabled. To build for a specific field or curve, you can pass the ",(0,r.yg)("inlineCode",{parentName:"p"},"-field=<field_name>")," or ",(0,r.yg)("inlineCode",{parentName:"p"},"-curve=<curve_name>")," flags:"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"./build.sh -curve=bn254\n")),(0,r.yg)("p",null,"Now you can import ICICLE into your project"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-go"},'import (\n    "github.com/ingonyama-zk/icicle/v3/wrappers/golang/core"\n    "github.com/ingonyama-zk/icicle/v3/wrappers/golang/runtime"\n)\n...\n')),(0,r.yg)("h3",{id:"building-with-precompiled-libs"},"Building with precompiled libs"),(0,r.yg)("p",null,"Download the frontend release binaries from our ",(0,r.yg)("a",{parentName:"p",href:"https://github.com/ingonyama-zk/icicle/releases"},"github release page"),", for example: icicle30-ubuntu22.tar.gz for ICICLE v3 on ubuntu 22.04"),(0,r.yg)("p",null,"Extract the libs and move them to the downloaded go dependency in your GOMODCACHE"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-sh"},"# extract frontend part\ntar xzvf icicle30-ubuntu22.tar.gz\ncp -r ./icicle/lib/* $(go env GOMODCACHE)/github.com/ingonyama-zk/icicle/v3@<version>/build/lib/\n")),(0,r.yg)("h2",{id:"running-tests"},"Running tests"),(0,r.yg)("p",null,"To run all tests, for all curves:"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"go test ./... -count=1\n")),(0,r.yg)("p",null,"If you wish to run test for a specific curve or field:"),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-bash"},"go test <path_to_curve_or_field> -count=1\n")),(0,r.yg)("h2",{id:"how-do-golang-bindings-work"},"How do Golang bindings work?"),(0,r.yg)("p",null,"The golang packages are binded to the libraries produced from compiling ICICLE using cgo."),(0,r.yg)("ol",null,(0,r.yg)("li",{parentName:"ol"},(0,r.yg)("p",{parentName:"li"},"These libraries (named ",(0,r.yg)("inlineCode",{parentName:"p"},"libicicle_curve_<curve>.a")," and ",(0,r.yg)("inlineCode",{parentName:"p"},"libicicle_field_<curve>.a"),") can be imported in your Go project to leverage the accelerated functionalities provided by ICICLE.")),(0,r.yg)("li",{parentName:"ol"},(0,r.yg)("p",{parentName:"li"},"In your Go project, you can use ",(0,r.yg)("inlineCode",{parentName:"p"},"cgo")," to link these libraries. Here's a basic example on how you can use ",(0,r.yg)("inlineCode",{parentName:"p"},"cgo")," to link these libraries:"))),(0,r.yg)("pre",null,(0,r.yg)("code",{parentName:"pre",className:"language-go"},'/*\n#cgo LDFLAGS: -L/path/to/shared/libs -licicle_device -lstdc++ -lm -Wl,-rpath=/path/to/shared/libs\n#include "icicle.h" // make sure you use the correct header file(s)\n*/\nimport "C"\n\nfunc main() {\n    // Now you can call the C functions from the ICICLE libraries.\n    // Note that C function calls are prefixed with \'C.\' in Go code.\n}\n')),(0,r.yg)("p",null,"Replace ",(0,r.yg)("inlineCode",{parentName:"p"},"/path/to/shared/libs")," with the actual path where the shared libraries are located on your system."),(0,r.yg)("h2",{id:"supported-curves-fields-and-operations"},"Supported curves, fields and operations"),(0,r.yg)("h3",{id:"supported-curves-and-operations"},"Supported curves and operations"),(0,r.yg)("table",null,(0,r.yg)("thead",{parentName:"table"},(0,r.yg)("tr",{parentName:"thead"},(0,r.yg)("th",{parentName:"tr",align:null},"Operation\\Curve"),(0,r.yg)("th",{parentName:"tr",align:"center"},"bn254"),(0,r.yg)("th",{parentName:"tr",align:"center"},"bls12_377"),(0,r.yg)("th",{parentName:"tr",align:"center"},"bls12_381"),(0,r.yg)("th",{parentName:"tr",align:"center"},"bw6-761"),(0,r.yg)("th",{parentName:"tr",align:"center"},"grumpkin"))),(0,r.yg)("tbody",{parentName:"table"},(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"MSM"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"G2"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u274c")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"NTT"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u274c")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"ECNTT"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u274c")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"VecOps"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"Polynomials"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u274c")))),(0,r.yg)("h3",{id:"supported-fields-and-operations"},"Supported fields and operations"),(0,r.yg)("table",null,(0,r.yg)("thead",{parentName:"table"},(0,r.yg)("tr",{parentName:"thead"},(0,r.yg)("th",{parentName:"tr",align:null},"Operation\\Field"),(0,r.yg)("th",{parentName:"tr",align:"center"},"babybear"))),(0,r.yg)("tbody",{parentName:"table"},(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"VecOps"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"Polynomials"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"NTT"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")),(0,r.yg)("tr",{parentName:"tbody"},(0,r.yg)("td",{parentName:"tr",align:null},"Extension Field"),(0,r.yg)("td",{parentName:"tr",align:"center"},"\u2705")))))}d.isMDXComponent=!0},1873:(e,n,a)=>{a(6540)}}]);