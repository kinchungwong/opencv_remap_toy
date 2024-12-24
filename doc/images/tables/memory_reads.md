# Memory read strategy for source sampler

### Characterization

  - Source sampler memory reads are constrained to a single contiguous OpenCV matrix buffer,
    which is CPU-based.
  - Regardless of the "cv::Mat::isContinuous()" flag, all OpenCV matrix have a backing
    buffer that is continuous by architecture design, even though the buffer might be
    shared among multiple OpenCV matrix views each with a region-of-interest (ROI).
  - In general, the source sampler will violate the strict aliasing rule of the compiler.
  - Source sampler does not write to the source matrix. Therefore, there is no
    read-after-write conflict as a result of strict aliasing rule violation.

  - Alignment
    - Unaligned access is assumed and is exploited to the maximum extent possible.
    - Wherever unaligned access is not permitted, it must be explicited noted as such.
  - For Intel Architecture (x86-32, x86-64)
    - Unaligned access is permitted for scalar memory operations.
    - Unaligned access instructions are available for SSE, AVX, AVX2, and AVX-512.
  - For ARM Architecture (arm64, NEON)
    - Unaligned access is permitted for scalar memory operations.
      - Alignment trap is a software-configurable hardware feature;
      - This trap is known to be disabled under all general purpose operating systems.
    - For SIMD operations, address alignment is based on the alignment size of the
      elements, i.e. aligned to the element size, not to the size of the full vector.
  - Atomicity
    - With regard to the source matrix, there is no read-after-write conflict.
    - Atomicity is not a concern for the source matrix.
    - However, write-tearing is a potential concern for the destination matrix; this
      will be discussed in the next chapter.

### Algorithm selection parameters


  

### Table - Memory reads strategy for source sampler

| Direct interpolation | Pixel size in bytes | Bytes read per row | Architecture | Strategy             |
| :------------------: | :-----------------: | :----------------: | :----------: | :------------------: |
| Bilinear             | max: 2              | 4                  | no AVX2      | 32-bit scalar        |
| Bilinear             | max: 2              | 4                  | AVX2         | 32-bit gather x 8    |
| Bilinear             | max: 2              | 4                  | AVX-512      | 32-bit gather x 16   |
| Bilinear             | max: 4              | 8                  | no AVX2      | 64-bit scalar        |
| Bilinear             | max: 4              | 8                  | AVX2         | 64-bit gather x 4    |
| Bilinear             | max: 4              | 8                  | AVX-512      | 64-bit gather x 8    |
| Bicubic              | max: 2              | 8                  | no AVX2      | 64-bit scalar        |
| Bicubic              | max: 2              | 8                  | AVX2         | 64-bit gather x 4    |
| Bicubic              | max: 2              | 8                  | AVX-512      | 64-bit gather x 8    |
| Bicubic              | max: 4              | 16                 | SSE2         | 128-bit v_load       |
| Bicubic              | max: 4              | 16                 | AVX2         | paired 64-bit gather |
| Bicubic              | max: 4              | 16                 | AVX-512      | paired 64-bit gather |

