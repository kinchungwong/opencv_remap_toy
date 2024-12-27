# Opencv-remap \- Status update \- 2024-12-16

---

### Code on GitHub

* \[\[**PUBLIC**\]\]  
* [https://github.com/kinchungwong/opencv\_remap\_toy/](https://github.com/kinchungwong/opencv_remap_toy/)

---

### Review of online discussions

#### Original ticket in issue tracker in opencv

* \[\[**OPEN**\]\]   
* \[\[**NO\_ACTIVITY**\]\]  
* [https://github.com/opencv/opencv/issues/7544](https://github.com/opencv/opencv/issues/7544)

#### Root cause analysis performed by AleksandrPanov

* \[\[**LINK\_TO\_COMMENT**\]\]   
* [https://github.com/opencv/opencv-python/issues/549\#issuecomment-938771861](https://github.com/opencv/opencv-python/issues/549#issuecomment-938771861)

#### Root cause analysis performed by myself

* \[\[**LINK\_TO\_COMMENT**\]\]   
* [https://github.com/opencv/opencv/issues/7544\#issuecomment-256166762](https://github.com/opencv/opencv/issues/7544#issuecomment-256166762)

#### Attempts by AleksandrPanov (OpenCV)

* [https://github.com/opencv/opencv/pull/20949](https://github.com/opencv/opencv/pull/20949)  
* [https://github.com/opencv/opencv/pull/20911](https://github.com/opencv/opencv/pull/20911)  
* Outcome: closed without merging

#### Root cause summary

* Facet 1: API limitation  
  * Remap accepts x and y coordinates passed in as 16-bit integers.  
  * Some OpenCV functions rely on this usage of remap.  
  * As such, these OpenCV functions are inherently limited, unless they switch to use wider integer types for the coordinates.  
* Facet 2: Pixel processing code implementation in Remap  
  * \[\[**UPDATE**\]\] It appears that the limitation isn't caused by SIMD code.  
  * AleksandrPanov's PR did not make changes to SIMD code, only C++ scalar code.  
  * SIMD code which might have contributed to this issue might have been removed.  
  * \[\[**MY\_VIEW**\]\]  
  * My root cause analysis was based on OpenCV 3.1.0 and OpenCV 3.4.1.  
  * If my memory was correct, the SIMD code was written in SSE2.  
  * Currently, there are no SSE2 intrinsics in imgwarp.  
  * Those codes might have been rewritten during the migration to "OpenCV universal intrinsics".

#### Older ticket migrated from old opencv issue tracker (frozen)

* [https://github.com/opencv/opencv/issues/4694](https://github.com/opencv/opencv/issues/4694)  
* Was migrated from:  
  * \[\[**DEADLINK**\]\] "https://code.opencv.org/issues/3522"

#### Duplicate ticket in opencv

* [https://github.com/opencv/opencv/issues/24075](https://github.com/opencv/opencv/issues/24075)

#### Related ticket in issue tracker in opencv-python

* [https://github.com/opencv/opencv-python/issues/549](https://github.com/opencv/opencv-python/issues/549)

#### Invalid attempts by others to fix the issues

* [https://github.com/opencv/opencv/pull/7555](https://github.com/opencv/opencv/pull/7555)  
* [https://github.com/opencv/opencv/pull/7560](https://github.com/opencv/opencv/pull/7560)  
* [https://github.com/opencv/opencv/pull/22987](https://github.com/opencv/opencv/pull/22987)

#### Issues in downstream project "DOI-USGS/ISIS3"

* [https://github.com/DOI-USGS/ISIS3/issues/4396](https://github.com/DOI-USGS/ISIS3/issues/4396)

#### Attempts to fix in "DOI-USGS/ISIS3" in C++ (FastGeom module) by "jessemapel" (mentored by "AustinSanders")

* [https://github.com/DOI-USGS/ISIS3/pull/4509](https://github.com/DOI-USGS/ISIS3/pull/4509)  
* Outcome: closed without merging  
* Problem:   
  * Copyright notice on my 2013 fix  
  * Too much code copied straight from my 2013 fix  
  * Company likely never responded to licensing inquiry  
  * Unlikely to pass copyright review

#### Attempts to fix in "DOI-USGS/ISIS3" in Python

* [https://github.com/DOI-USGS/ISIS3/pull/5091](https://github.com/DOI-USGS/ISIS3/pull/5091)  
* Outcome: merged  
* Status: released

---

### Non-OpenCV alternatives to remap

#### Using scikit-image (skimage)

* \[\[**RECOMMENDED**\]\]  
* Syntax: "skimage.transform.warp(...)"  
* [https://stackoverflow.com/questions/69367152/replace-cv2-warpperspective-for-big-images/69396522](https://stackoverflow.com/questions/69367152/replace-cv2-warpperspective-for-big-images/69396522)

#### Using OpenGL

* [https://stackoverflow.com/questions/33018652/how-to-use-opengl-to-emulate-opencvs-warpperspective-functionality-perspective](https://stackoverflow.com/questions/33018652/how-to-use-opengl-to-emulate-opencvs-warpperspective-functionality-perspective)

#### Using math from first principles

* \[\[**NOTE**\]\] Good introduction to math for beginners, but no new insights here.  
* [https://stackoverflow.com/questions/56325847/alternative-to-opencv-warpperspective](https://stackoverflow.com/questions/56325847/alternative-to-opencv-warpperspective)

---

### OpenCV SIMD intrinsics

[https://docs.opencv.org/4.x/df/d91/group\_\_core\_\_hal\_\_intrin.html](https://docs.opencv.org/4.x/df/d91/group__core__hal__intrin.html)  
---

### Information on AVX-512

\[\[**TODO**\]\]  
---

### Unsorted links on AVX-512

* [https://stackoverflow.com/questions/61702041/how-can-i-gather-single-bytes-with-avx512-intrinsics-given-a-vector-of-int-offs](https://stackoverflow.com/questions/61702041/how-can-i-gather-single-bytes-with-avx512-intrinsics-given-a-vector-of-int-offs)  
* [https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-9/intrinsics-for-int-gather-and-scatter-ops.html](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-9/intrinsics-for-int-gather-and-scatter-ops.html)  
* [https://uops.info/html-lat/ADL-P/VPGATHERDD\_YMM\_VSIB\_YMM\_YMM-Measurements.html\#lat2-%3E1\_mem](https://uops.info/html-lat/ADL-P/VPGATHERDD_YMM_VSIB_YMM_YMM-Measurements.html#lat2-%3E1_mem)  
* [https://en.wikipedia.org/wiki/AVX-512](https://en.wikipedia.org/wiki/AVX-512)  
* [https://en.wikipedia.org/wiki/Advanced\_Vector\_Extensions\#New\_instructions\_2](https://en.wikipedia.org/wiki/Advanced_Vector_Extensions#New_instructions_2)  
* [https://github.com/twest820/AVX-512](https://github.com/twest820/AVX-512)  
* [https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/intrinsics-for-integer-conversion-operations.html](https://www.intel.com/content/www/us/en/docs/cpp-compiler/developer-guide-reference/2021-8/intrinsics-for-integer-conversion-operations.html)

---

