# Opencv-remap \- Status update \- 2024-12-24

---

#### Previous documents

* [Opencv-remap - Status update - 2024-12-16](./Opencv-remap%20-%20Status%20update%20-%202024-12-16.md)

---

### This week's goal \- Implement U8C1 Bicubic sampler with AVX2 gather instructions

#### Inputs

* Pixel format is 8-bit unsigned integer, single channel (grayscale).  
* A source tile and a destination tile is provided.  
* Two matrices, containing the real-valued source X and Y coordinates for each destination pixel, are provided.

#### Specifics

* Bounds-checking has been taken care of.  
* Each destination pixel (at integer coordinates) has a corresponding source point, with real-valued coordinates.  
* To derive the destination pixel value, the nearest 4x4 source pixels are Bicubic interpolated.  
* The multiplicative weights for Bicubic interpolation can be either precomputed (loaded from a table at runtime), or computed at the spot. Both should be implemented and evaluated.  
* Bicubic interpolations are separable by axes. What this means is that each evaluation requires 4 weights in the x axis, and 4 weights in the y axis.  
* The weights required for the axes are solely determined by the fractional parts of the real-valued coordinates for the corresponding source point.  
* In implementing the table for precomputed weights,   
  * The code should allow quantizations to be configurable at compile-time.  
  * Code for table initialization should be implemented.

#### Specifics on the use of AVX2 gather instructions

* To read the nearest 4x4 source pixels,   
  * Data has to be read from four consecutive rows in the source tile.  
  * From each row, 4 pixel values are read.  
  * These 4 pixel values are also consecutive; thus, a 32-bit load is sufficient.  
  * The start addresses of these 32-bit loads are not aligned on a 32-bit boundary. Instead, they are aligned on an 8-bit boundary.  
* One of the AVX2 gather instructions has the following form:  
  * (Without loss of generality, the runtime environment is assumed to be 64-bit.)  
  * A 64-bit pointer is provided as the base address.  
  * Four unsigned (non-negative) offsets, each 32-bit, are packed into a 128-bit vector.  
  * An encoded parameter called "scale" is specified. For our purposes, it is set to 1\.  
  * The four offsets are multiplied by the scale. For our purposes, we would keep those offsets unchanged.  
  * Data is gathered from the four addresses, as in (base address \+ the k-th offset) for k in 0, 1, 2, 3\.  
* Once gathered, the four pixel values on each row, currently packed in 32-bit words in the 128-bit vector, are widened in preparation for pixel value interpolation arithmetics.  
* Separately, in order to load the multiplicative weights for bicubic interpolation, these too are loaded from memory using the same AVX2 gather instructions.  
* The same table for interpolation weights is used for X and Y axes. However, the real-valued source point coordinates may have different fractional parts for its X and Y axes, which means they need to be loaded from two different entries on the table.  
* Each weight is stored as a 16-bit signed integer. This is due to the fact that some weights have a magnitude greater than \+/- 1.0 (plus or minus one point zero).  
* In total, two 64-bit loads are needed, one for each axis.  
* This, too, will be handled with AVX2 gather instructions, specifically the 64-bit datum form.

---

### Update \- 2024-12-25 \- morning

#### Pushed

* Pushed "7e77d4f", a math kernel for bicubic u8c1 mixing.  
  * Function name "bicubic\_mix\_u8\_v128\_sse41"  
  * This function takes packed 4x4 source pixels in one v128 register, packed 4+4 coefficients in another.  
  * Tested and validated.  
  * Disassembly looks legit.  
* Pushed "83b6f6a", calls the test function from main.

#### Junked

* Local branch "junk\_20241225"  
* Totally botched and not worth keeping.  
* Key differences from the accepted changes  
  * In the accepted push, row coefficients are splat-and-multiplied, which is far more efficient than the junked code.  
  * In the accepted push, memory operations are eliminated from the math kernel, making it pure.

#### To-do

* Design and implement gathering, using the junked code as a "what to do" and "what not to do".

#### Important to-do (strategic planning)

* Memory throughput and latency testing should be done before committing to any new designs for the gatherer.  
* We don't want the code base to be cluttered with junk code.

---

### Memory performance characterizations for various u8c1 bicubic gather designs

#### Readings

* \[\[**DOWNLOADED**\]\] "57647\_zen4\_sog.pdf" \- AMD Zen4 optimization guide

#### Brainstorming

* Question: What micro-benchmarks are needed?

---

### Reminder \- OpenCV Matrix region-of-interest handling

\[\[**MUST\_READ**\]\]

* [https://github.com/opencv/opencv/issues/8304](https://github.com/opencv/opencv/issues/8304)  
* Refer to the last diagram in the thread.

---

### Updates on u8c1 bicubic gather based on latest design \- TBD

\[\[**PENDING**\]\]  
\[\[**SUBJECT\_TO\_CHANGE**\]\]  
This section will be updated once we have memory throughput and latency testing results.

#### Guiding principle

* With respect to the L1 data cache size, the initial recommendation is to process in 8x8 output tiles.

#### Draft

* During the early speculative coordinates-checking stage,  
  * Only the x and y coordinates of the four corner pixels on the 8x8 tile are checked, using scalar C++ code.  
* During the full coordinates-checking stage,   
  * Values in srcxq and srcyq are checked, one row at a time.  
  * Memory reads per row of 8 pixels: (32bit x 8w x 1h x (1x \+ 1y) \= 64 bytes)  
  * Total memory reads: (32bit x 8w x 8h x (1x \+ 1y) \= 512 bytes)  
  * For each row, if all coordinates are safely within bounds, the row is SIMD-accelerated, without any further range checks.  
  * Otherwise, the row is handled with scalar C++ code.  
* The scalar C++ code will be monolithic.  
* The SIMD code will be divided into stages.  
  * Each stage is a function call with the suffix "r1c8", meaning that each call processes 1 row of 8 pixels.  
* Each corresponding source coordinate point is converted into:  
  * A center pixel address in the source tile;  
  * Fractional parts of x and y values, normalized to a scale of 256 (8 bits);  
* During the source neighborhood gathering stage,  
  * A 4x4 patch of 8-bit source pixels are loaded, using AVX gathering instructions.   
  * The load addresses are merely byte-aligned.  
  * This gather operation requires the use of a precomputed absolute address offset vector.   
  * The base address is computed from the center pixel address, adjusted by (\*\*subtracting\*\*) (src\_step \+ 1\)  
  * This is because AVX gather instructions can only accept non-negative offsets.  
  * The precomputed absolute address offset vector is thus (k \* src\_step), for (k) in (0, 1, 2, 3).  
  * After gathering, they are packed into a 128-bit vector.  
* During the bicubic coefficient gathering stage,  
  * Two 64-bit sets of values are gathered, using the fractional x and y values as table indices respectively.  
  * Each 64-bit set contains 4 signed 16-bit fixed-point weights, to a scale of 256 (q8).  
  * Total table size is (64-bit x 256 entries \= 2048 bytes).  
  * After gathering, they are packed into a 128-bit vector.

---

### Update \- 2024-12-25 \- evening

* \[\[**PROGRESS**\]\]  
* Class "BicubicDestTileGeneratorSimd"  
* Class "BicubicDestTileGeneratorSimdMixin"  
* Code is very messy, but is able to run and produces what looks like a legit result.

---

### Policy \- 2024-12-26 \- Regarding experimental code unfit for push

#### Project policy

* Starting immediately, all folders with the exact name "attic" or "no\_commit" anywhere in the project (at all depths) will be git-ignored.  
* Code that refers to symbols located in any header or source files residing in these git-ignored folders shall not be pushed to the main repo.  
* However, such code shall be allowed on local branches on personal clones.

#### Rationale \- as a hack for untracked stashing

* Stashing is full of problems.  
* Stashing simply doesn't fulfill the requirements of managing experimental code that need orthogonal coexistence.  
* Instead of stashing,  
* Where necessary, we just create short-lived local branches.  
* Just remember to clean up and delete them soon after.  
* Where absolutely necessary, we just lay it bare those experimental files on the local file system, git-ignored.

---

### Policy \- 2024-12-26 \- Regarding the mixin pattern

\[\[**TENTATIVE**\]\]

* [https://www.perplexity.ai/search/prepare-a-section-of-c-coding-AyRgGjSeTv.5ZSVjtEvjog](https://www.perplexity.ai/search/prepare-a-section-of-c-coding-AyRgGjSeTv.5ZSVjtEvjog)

---

### Strategic next step \- 2024-12-26 \- Refactoring \- Separation of Remapping and Remapper

#### Notice on name simplification

* \[\[**IMPORTANT**\]\]  
* To improve code brevity, the "re-" prefix is always omitted in source code.  
* Thus, "remapping" will appear as "mapping"; "remapper" will appear as "mapper".

#### Notice on name simplification \- undecided

* Further shortening is possible.  
* For illustrative purposes only, "mapping" could become "mpng", "mapper" could become "mpr".   
* There is no decision on this yet.   
* It is unlikely that "mpng" can be used \- it conflicts with an existing technical use.

#### Definition \- remapping

* A "remapping" is the definition of a mathematical object that specifies how source coordinates and destination coordinates are mapped.

#### Definition \- remapper

* A remapper is an image processing (transformation) step, in which a source image and a remapping is given, and a destination image is produced.

#### Remapping \- key characteristics

* A remapping is only concerned with the two coordinate spaces, both of which are two-dimensional and real-valued.  
* The remapping has to be chosen by the user.  
* The user can use standard remappings (with configurable parameters) provided in the library.  
* The user shall be allowed to implement their own remapping.

#### Remapping \- what it is not

* A remapping is never concerned with: pixel formats.

#### Remapping \- mathematical singularities

* The remapping interface must provide a way to inform its users of mathematical singularities in that remapping.  
* Users can query for the approximate locations of all singularities in source and destination coordinates.  
* Alternatively, users can pass in a query rectangle to check for the presence of singularities.

#### Remapping \- direction and invertibility

* Customarily, the remapping function goes in the backward direction  
* It takes the destination coordinates, and produces the corresponding source coordinates.  
* The availability of inverse mapping depends on the mathematical nature of the remapping.  
* An algorithm for constructing numerically approximated inverse mapping is technically possible.

#### Remapping \- design for performance

* Both scalar and arrayed versions of remapping must be defined (on the interface) and implemented (by the class).  
* Optionally, the arrayed version can use SIMD internally.  
  * If so, such code is responsible for checking hardware capability and providing fallback code paths.  
* User-defined remapping may provide only the scalar function, and then use an adapter to fulfill the arrayed function.  
* Optionally, multiple versions of the scalar-to-arrayed adapter can be provided; …  
  * … some of which may use compiler-specific directives to achieve inlining and loop vectorization.

#### Remapper \- key characteristics

* Must handle a wide variety of pixel formats.  
* Must control code bloat, despite the need to support these pixel formats.

#### Remapper \- niche pixel formats used by downstream users

* Pixel formats. Some are primarily used by astronomical communities that depend on OpenCV; these needs have to be covered.

#### Remapper \- partitioning and tiling

#### Remapper \- multithreaded execution

#### Remapper \- source pre-treatment

* In some downstream uses, the source image is pre-processed by non-convolutional means.  
* Examples are:  
* Anisotropic diffusion  
* Deep neural network super-resolution

---

### Notes on Mapping and Mapper interface design \- 2024-12-26

#### Mapping \- requires x and y coordinates to be stored interleaved in arrays and matrices

* After deliberation, it is decided that the x and y coordinates for both source and destination will be passed across function boundaries in the following forms:  
* (As OpenCV matrices) cv::Mat2i or cv::Mat2f  
* (As plain C++ pointer and size combo) (size\_t pair\_count, int\* or float\* arr) where the arr contains interleaved x and y coordinates  
* The non-interleaved member functions will be removed from the interface.

#### Rationale

* All operations on Mapping and Mapper require x and y coordinates to be read together (in tandem); there is no meaningful operation from reading either one alone.  
* In order to make seamless integration between the OpenCV matrix interface and the plain C-style array interface, either they should both be interleaved, or vice versa.  
* We want to prevent interface bloat.  
* We decided to keep the interleaved methods, which means we'll remove the non-interleaved methods.

---

