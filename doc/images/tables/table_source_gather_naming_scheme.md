### Table - naming scheme for source gather functions

  - Byte size per arithmetic element (e.g. 1 = byte, signed or unsigned)
  - Number of arithmetic elements to be read consecutively from each row
    - Nearest: 1 x channels
    - Bilinear: 2 x channels
    - Bicubic: 4 x channels
    - Lanczos3: 6 x channels
    - Lanczos4: 8 x channels
  - Instruction load width per element (not necessarily same as arithmetic element)
    - epi8: 8-bit
    - epi32: 32-bit
    - epi64: 64-bit
  - Vectorization width (total width of a widest SIMD load)
    - SSE2, NEON: 128-bit
    - AVX2: 256-bit
    - AVX-512: 512-bit
  - Pseudo-vectorizations
    - When vectorization is not available, hard-coded hand-unrolled loads are used.
  - CPU capability flags
    - Post-SSE2 flags required for arithmetic processing (e.g. PMULLD requires SSE41)
    - Specific AVX-512 sub-flags required for arithmetic processing (e.g. BW, DQ, VBMI, VBMI2, IFMA)

### Table - function naming scheme

```{fmt}_p{b}x{c}_f{fw}x{fh}_o{w}x{h}_v{vlen}_{arch}```

Where:
  - ```{fmt}```: OpenCV pixel format shorthand, with the arithmetic kind letter moved upfront.
    - Example: U8C3 (corresponds to OpenCV 8UC3)
    - Example: F32C4 (corresponds to OpenCV 32FC4)
  - ```{b}```: number of bytes per channel
    - Same as: number of bytes per arithmetic element
  - ```{c}```: number of channels per pixel
  - ```{fw}```: interpolation filter width
    - Same as: number of horizontal neighbors to be sampled from the source for interpolation
  - ```{fh}```: interpolation filter height
  - ```{w}```: width of output to be produced in each batch
  - ```{h}```: height of output to be produced in each batch
  - ```{vlen}```: widest SIMD register width directly involved in calculations, in bits
  - ```{arch}```: instruction architecture requirements

Example:
  - ```u8c1_p1x1_f4x4_o1x1_v128_avx2```
    - Reads and produces interpolated CV_8UC1 pixels
    - (Same information, presented in bytes) (1x1)
    - Bicubic separable interpolation (4x4)
    - Produces one output pixel at a time (1x1)
    - Widest SIMD registers used are 128-bit
    - Requires avx2 support for 32x4 gather instructions
