Todo list
=========

- The KMP table computation can be linear instead of quadratic
- It may be a good idea to abstract KMP to ff_kmp
- Write unit tests (soundness and performance)
- Memory complexity could by O(1):
  - all decisions may be taken using 8 bytes (mark excluded)
  - it is an error if last prefix is not closed
