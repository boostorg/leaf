# Print File Example

This directory has three versions of the same simple program, which reads a
file, prints it to standard out and handles errors using LEAF, each using a
different variation on error handling:

* [print_file_leaf_result.cpp](./print_file_leaf_result.cpp) reports errors with
  `boost::leaf::result<T>`, using an error code `enum` for classification of failures.

* [print_file_system_result.cpp](./print_file_system_result.cpp) is the same as
  above, but using `boost::system::result<T>` instead of `boost::leaf::result<T>`.
  This demonstrates the ability of LEAF to transport arbitrary error objects using an
  external result type, rather than `boost::leaf::result<T>`.

* [print_file_outcome_result.cpp](./print_file_outcome_result.cpp) is the same as
  above, but using `boost::outcome::result<T>` instead of `boost::leaf::result<T>`.

* [print_file_eh.cpp](./print_file_eh.cpp) throws on error, using an error code
  `enum` for classification of failures.
