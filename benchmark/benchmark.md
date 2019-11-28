# Benchmark

The LEAF github repository contains two similar benchmarking programs, one using LEAF, the other using Boost Outcome, that simulate transporting error objects across 32 levels of function calls, measuring the performance of the two libraries.

Official library documentation:
* LEAF: https://zajo.github.io/leaf
* Boost Outcome: https://www.boost.org/doc/libs/release/libs/outcome/doc/html/index.html

## Library design considerations

It is important to understand that LEAF and Outcome serve similar purpose but follow very different design philosophy. The benchmarks are comparing apples and oranges.

The main design difference is that LEAF has a very strong bias towards the use case where, after a call to a function which may fail, any failures are not handled but instead forwarded to the caller. When we merely check "do we have a failure", LEAF does not bother to make available the error objects associated with the failure. This saves a lot of cycles.

In contrast, Outcome always delivers error objects to each level of function call, even when the user is not handling errors at that point, and therefore has no intention of accessing them.

This difference is reflected in how each library is used. The simple "check-only" case is nearly identical, e.g.:

```c++
// Outcome
{
	OUTCOME_TRY(v, f()); // Check for errors, forward failures to the caller
	// If control reaches here, v is the successful result (the call succeeded).
}
```

```c++
// LEAF
{
	LEAF_AUTO(v, f()); // Check for errors, forward failures to the caller
	// If control reaches here, v is the successful result (the call succeeded).
}
```

However, when we want to handle failures, in Outcome accessing the error object (which is always available in the returned object) is a simple continuation of the error check:

```c++
// Outcome
if( auto r = f() )
	return r.value()+1; // No error
else
{	// Error!
	switch( r.error() )
	{
		error_enum::error1: /* handle */ break;
		error_enum::error2: /* handle */ break;
		default: return r.as_failure();
	}
}
```

When using LEAF, we must explicitly state our intention to handle some errors, not just check for failures:

```c++
// LEAF
return leaf::try_handle_some(
	[]() -> leaf::result<T>
	{
		LEAF_AUTO(v, f());
		// No error, use v
	},
	[]( leaf::match<error_enum, error_enum::error1> )
	{
		/* handle */
	},
	[]( leaf::match<error_enum, error_enum::error2> )
	{
		/* handle */
	} );
```

The use of `try_handle_some` reserves storage on the stack for the error object types being handled (in this case, `error_enum`). If the failure is either `error_enum::error1` or `error_enum::error2`, the matching error handling lambda is invoked. Otherwise, the failure is automatically forwarded to the caller.

## Code generation considerations

Benchmarking C++ programs is tricky, because we want to prevent the compiler from optimizing out things it shouldn't normally be able to optimize in a real program, yet we don't want to interfere with "legitimate" optimizations.

The primary approach we use to prevent the compiler from optimizing everything out to nothing is to base all computations on a call to `std::rand()`. This adds cycles, but it also makes the benchmark more realistic, since functions which may legitimately fail should do _some_ real work.

When benchmarking error handling, it makes sense to measure the time it takes to return a result or error across multiple stack frames. This calls for disabling inlining. On the other hand, in C++ inlining is often possible, so that case must be measured as well.

The technique used to disable inlining in this benchmark is to mark functions as `__attribute__((noinline))` / `__declspec(noinline)`. This is imperfect, because optimizers can still peek into the body of the function and optimize things out, as is seen in this example:

```c++
__attribute__((noinline)) int val() {return 42;}

int main() {
    return val();
}
```

Which on clang 9 outputs:

```assembly_x86
val():
        mov     eax, 42
        ret
main:
        mov     eax, 42
        ret
```

It does not appear that anything like this is occurring in our case, but it is still a possibility.

> NOTES:
>
> - For benchmarking, both programs are compiled with exception handling disabled.
> - LEAF is able to work with external `result<>` types. The benchmark uses `leaf::result<T>`.

## Show me the code!

The following source:

```C++
leaf::result<int> f();

leaf::result<int> g()
{
    LEAF_AUTO(x, f());
    return x+1;
}
```

Generates this code on clang ([Godbolt](https://godbolt.org/z/4AtHMk)):

```assembly_x86
g():                                  # @g()
        push    rbx
        sub     rsp, 32
        mov     rbx, rdi
        lea     rdi, [rsp + 8]
        call    f()
        mov     eax, dword ptr [rsp + 8]
        mov     ecx, eax
        and     ecx, 3
        cmp     ecx, 2
        je      .LBB0_4
        cmp     ecx, 3
        jne     .LBB0_2
        mov     eax, dword ptr [rsp + 16]
        add     eax, 1
        mov     dword ptr [rbx], 3
        mov     dword ptr [rbx + 8], eax
        mov     rax, rbx
        add     rsp, 32
        pop     rbx
        ret
.LBB0_4:
        mov     dword ptr [rbx], 2
        movups  xmm0, xmmword ptr [rsp + 16]
        mov     qword ptr [rsp + 24], 0
        movups  xmmword ptr [rbx + 8], xmm0
        mov     qword ptr [rsp + 16], 0
        mov     rax, rbx
        add     rsp, 32
        pop     rbx
        ret
.LBB0_2:
        mov     dword ptr [rbx], eax
        mov     rax, rbx
        add     rsp, 32
        pop     rbx
        ret
```

> Description:
>
> * The default no-jump path is the happy path (ends at the first `ret`); the returned `result<T>` holds the `int` discriminant and the `T` (`int` in this case).
>
> * `.LBB0_2`: Regular failure; the returned `result<T>` object holds only the `int` discriminant.
>
> * `.LBB0_4`: Failure; the returned `result<T>` holds the `int` discriminant and a `std::shared_ptr<leaf::polymorphic_context>` (used to hold error objects transported from another thread).

Note that `f` is undefined, hence the `call` instruction. Predictably, if we provide a trivial definition for `f`:

```C++
leaf::result<int> f()
{
    return 42;
}

leaf::result<int> g()
{
    LEAF_AUTO(x, f());
    return x+1;
}
```

We get:

```assembly_x86
g():                                  # @g()
        mov     rax, rdi
        mov     dword ptr [rdi], 3
        mov     dword ptr [rdi + 8], 43
        ret
```

With a less trivial definition of `f`:

```C++
leaf::result<int> f()
{
    if( rand()%2 )
        return 42;
    else
        return leaf::new_error();
}

leaf::result<int> g()
{
    LEAF_AUTO(x, f());
    return x+1;
}
```

We get ([Godbolt](https://godbolt.org/z/4P7Jvv)):

```assembly_x86
g():                                  # @g()
        push    rbx
        mov     rbx, rdi
        call    rand
        test    al, 1
        jne     .LBB1_5
        mov     eax, dword ptr fs:[boost::leaf::leaf_detail::id_factory<void>::next_id@TPOFF]
        test    eax, eax
        je      .LBB1_3
        mov     dword ptr fs:[boost::leaf::leaf_detail::id_factory<void>::next_id@TPOFF], 0
        jmp     .LBB1_4
.LBB1_5:
        mov     dword ptr [rbx], 3
        mov     dword ptr [rbx + 8], 43
        mov     rax, rbx
        pop     rbx
        ret
.LBB1_3:
        mov     eax, 4
        lock            xadd    dword ptr [rip + boost::leaf::leaf_detail::id_factory<void>::counter], eax
        add     eax, 4
.LBB1_4:
        and     eax, -4
        or      eax, 1
        mov     dword ptr fs:[boost::leaf::leaf_detail::id_factory<void>::last_id@TPOFF], eax
        mov     dword ptr [rbx], eax
        mov     rax, rbx
        pop     rbx
        ret
```

Above, the call to `f()` is inlined, most of the code is from the initial error reporting machinery in LEAF.

## Benchmark matrix dimensions

The benchmark matrix has 4 dimensions:

1. Check only vs. handle some errors:

	a. Each function calls the lower level function, does work on success, forwards errors to the caller, but does not handle them (the root of the call chain handles the error).

	b. Each fourth level of function call doesn't only check, but also handles some errors, forwarding other errors to the caller.

2. Error object type:

	a. The error object transported in case of a failure is of type `e_error_code`, which is a simple `enum`.

	b. The error object transported in case of a failure is of type `struct e_system_error { e_error_code value; std::string what; }`.

	c. The error object transported in case of a failure is of type `e_heavy_payload`, a `struct` of size 4096.

3. Error rate: 2%, 50%, 98%

4. Inline vs. noinline (32 levels of function calls).

## Source code

[deep_stack_leaf.cpp](deep_stack_leaf.cpp)

[deep_stack_outcome.cpp](deep_stack_outcome.cpp)

## Godbolt

LEAF provides a single header which makes it very easy to use online. To see the generated code for the benchmark program, you can copy and paste the following into Godbolt:

```c++
#include "https://raw.githubusercontent.com/zajo/leaf/master/include/boost/leaf/all.hpp"
#include "https://raw.githubusercontent.com/zajo/leaf/master/benchmark/deep_stack_leaf.cpp"
```

See https://godbolt.org/z/DTk4N4.

## Build options

To build both versions of the benchmark program, the compilers are invoked using the following command line options:

* `-std=c++17`: Required by Outcome (LEAF only requires C++11);
* `-fno-exceptions`: Disable exception handling;
* `-O3`: Maximum optimizations;
* `-DNDEBUG`: Disable asserts.

In addition, the LEAF version is compiled with:

* `-DLEAF_DIAGNOSTICS=0`: Disable diagnostic information for error objects not recognized by the program. This is a debugging feature, see [Configuration Macros](https://zajo.github.io/leaf/#_configuration_macros).

## Results

Below is the output the benchmark programs running on a MacBook Pro. The tables show the elapsed time for returning a result across 32 levels of function calls, depending on the error type, the action taken at each level, whether inlining is enabled, and the rate of failures. In addition, the programs generate a `benchmark.csv` file in the current working directory.


The following tables show elapsed time

#### 10000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096 (clang, LEAF):

Error type      | At each level      | Inlining | Rate | μs
----------------|--------------------|----------|------:|-------:
e_error_code    | LEAF_AUTO          | Disabled |    2% |     920
e_error_code    | LEAF_AUTO          | Enabled  |    2% |     135
e_error_code    | try_handle_some    | Disabled |    2% |    1407
e_error_code    | try_handle_some    | Enabled  |    2% |     708
e_error_code    | LEAF_AUTO          | Disabled |   50% |     916
e_error_code    | LEAF_AUTO          | Enabled  |   50% |     215
e_error_code    | try_handle_some    | Disabled |   50% |    1467
e_error_code    | try_handle_some    | Enabled  |   50% |     851
e_error_code    | LEAF_AUTO          | Disabled |   98% |     803
e_error_code    | LEAF_AUTO          | Enabled  |   98% |     204
e_error_code    | try_handle_some    | Disabled |   98% |    1671
e_error_code    | try_handle_some    | Enabled  |   98% |     907
e_system_error  | LEAF_AUTO          | Disabled |    2% |    1255
e_system_error  | LEAF_AUTO          | Enabled  |    2% |     204
e_system_error  | try_handle_some    | Disabled |    2% |    1482
e_system_error  | try_handle_some    | Enabled  |    2% |     731
e_system_error  | LEAF_AUTO          | Disabled |   50% |    1147
e_system_error  | LEAF_AUTO          | Enabled  |   50% |     362
e_system_error  | try_handle_some    | Disabled |   50% |    1821
e_system_error  | try_handle_some    | Enabled  |   50% |     858
e_system_error  | LEAF_AUTO          | Disabled |   98% |     977
e_system_error  | LEAF_AUTO          | Enabled  |   98% |     211
e_system_error  | try_handle_some    | Disabled |   98% |    1585
e_system_error  | try_handle_some    | Enabled  |   98% |     952
e_heavy_payload | LEAF_AUTO          | Disabled |    2% |    1089
e_heavy_payload | LEAF_AUTO          | Enabled  |    2% |     154
e_heavy_payload | try_handle_some    | Disabled |    2% |    1405
e_heavy_payload | try_handle_some    | Enabled  |    2% |     824
e_heavy_payload | LEAF_AUTO          | Disabled |   50% |    1476
e_heavy_payload | LEAF_AUTO          | Enabled  |   50% |     576
e_heavy_payload | try_handle_some    | Disabled |   50% |    2494
e_heavy_payload | try_handle_some    | Enabled  |   50% |    1338
e_heavy_payload | LEAF_AUTO          | Disabled |   98% |    1644
e_heavy_payload | LEAF_AUTO          | Enabled  |   98% |     931
e_heavy_payload | try_handle_some    | Disabled |   98% |    2646
e_heavy_payload | try_handle_some    | Enabled  |   98% |    1676

#### 10000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096 (clang, Outcome):

Error type      | At each level      | Inlining | Rate  | μs
----------------|--------------------|----------|------:|-------:
e_error_code    | OUTCOME_TRY        | Disabled |    2% |    2129
e_error_code    | OUTCOME_TRY        | Enabled  |    2% |    2245
e_error_code    | Handle some errors | Disabled |    2% |    2102
e_error_code    | Handle some errors | Enabled  |    2% |    2157
e_error_code    | OUTCOME_TRY        | Disabled |   50% |    3994
e_error_code    | OUTCOME_TRY        | Enabled  |   50% |    4043
e_error_code    | Handle some errors | Disabled |   50% |    2958
e_error_code    | Handle some errors | Enabled  |   50% |    2994
e_error_code    | OUTCOME_TRY        | Disabled |   98% |    5533
e_error_code    | OUTCOME_TRY        | Enabled  |   98% |    5573
e_error_code    | Handle some errors | Disabled |   98% |    3754
e_error_code    | Handle some errors | Enabled  |   98% |    3824
e_system_error  | OUTCOME_TRY        | Disabled |    2% |    2144
e_system_error  | OUTCOME_TRY        | Enabled  |    2% |    2158
e_system_error  | Handle some errors | Disabled |    2% |    2130
e_system_error  | Handle some errors | Enabled  |    2% |    2187
e_system_error  | OUTCOME_TRY        | Disabled |   50% |    3954
e_system_error  | OUTCOME_TRY        | Enabled  |   50% |    3996
e_system_error  | Handle some errors | Disabled |   50% |    3210
e_system_error  | Handle some errors | Enabled  |   50% |    3369
e_system_error  | OUTCOME_TRY        | Disabled |   98% |    5422
e_system_error  | OUTCOME_TRY        | Enabled  |   98% |    5247
e_system_error  | Handle some errors | Disabled |   98% |    4278
e_system_error  | Handle some errors | Enabled  |   98% |    4126
e_heavy_payload | OUTCOME_TRY        | Disabled |    2% |   25931
e_heavy_payload | OUTCOME_TRY        | Enabled  |    2% |   26290
e_heavy_payload | Handle some errors | Disabled |    2% |   26060
e_heavy_payload | Handle some errors | Enabled  |    2% |   26176
e_heavy_payload | OUTCOME_TRY        | Disabled |   50% |   43802
e_heavy_payload | OUTCOME_TRY        | Enabled  |   50% |   42883
e_heavy_payload | Handle some errors | Disabled |   50% |   42538
e_heavy_payload | Handle some errors | Enabled  |   50% |   42590
e_heavy_payload | OUTCOME_TRY        | Disabled |   98% |   58263
e_heavy_payload | OUTCOME_TRY        | Enabled  |   98% |   57450
e_heavy_payload | Handle some errors | Disabled |   98% |   58248
e_heavy_payload | Handle some errors | Enabled  |   98% |   57814

#### 10000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096 (gcc, LEAF):

Error type      | At each level      | Inlining | Rate | μs
----------------|--------------------|----------|------:|-------:
e_error_code    | LEAF_AUTO          | Disabled |    2% |    1973
e_error_code    | LEAF_AUTO          | Enabled  |    2% |      76
e_error_code    | try_handle_some    | Disabled |    2% |    2381
e_error_code    | try_handle_some    | Enabled  |    2% |     356
e_error_code    | LEAF_AUTO          | Disabled |   50% |    1741
e_error_code    | LEAF_AUTO          | Enabled  |   50% |     235
e_error_code    | try_handle_some    | Disabled |   50% |    2681
e_error_code    | try_handle_some    | Enabled  |   50% |     555
e_error_code    | LEAF_AUTO          | Disabled |   98% |    1296
e_error_code    | LEAF_AUTO          | Enabled  |   98% |     234
e_error_code    | try_handle_some    | Disabled |   98% |    2389
e_error_code    | try_handle_some    | Enabled  |   98% |     599
e_system_error  | LEAF_AUTO          | Disabled |    2% |    1796
e_system_error  | LEAF_AUTO          | Enabled  |    2% |      76
e_system_error  | try_handle_some    | Disabled |    2% |    2511
e_system_error  | try_handle_some    | Enabled  |    2% |     369
e_system_error  | LEAF_AUTO          | Disabled |   50% |    1643
e_system_error  | LEAF_AUTO          | Enabled  |   50% |     249
e_system_error  | try_handle_some    | Disabled |   50% |    2338
e_system_error  | try_handle_some    | Enabled  |   50% |     574
e_system_error  | LEAF_AUTO          | Disabled |   98% |    1205
e_system_error  | LEAF_AUTO          | Enabled  |   98% |     263
e_system_error  | try_handle_some    | Disabled |   98% |    2316
e_system_error  | try_handle_some    | Enabled  |   98% |     672
e_heavy_payload | LEAF_AUTO          | Disabled |    2% |    1907
e_heavy_payload | LEAF_AUTO          | Enabled  |    2% |      91
e_heavy_payload | try_handle_some    | Disabled |    2% |    2069
e_heavy_payload | try_handle_some    | Enabled  |    2% |     392
e_heavy_payload | LEAF_AUTO          | Disabled |   50% |    1982
e_heavy_payload | LEAF_AUTO          | Enabled  |   50% |     831
e_heavy_payload | try_handle_some    | Disabled |   50% |    2579
e_heavy_payload | try_handle_some    | Enabled  |   50% |    1139
e_heavy_payload | LEAF_AUTO          | Disabled |   98% |    2032
e_heavy_payload | LEAF_AUTO          | Enabled  |   98% |    1579
e_heavy_payload | try_handle_some    | Disabled |   98% |    2806
e_heavy_payload | try_handle_some    | Enabled  |   98% |    1682

#### 10000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096 (gcc, Outcome):

Error type      | At each level      | Inlining | Rate  | μs
----------------|--------------------|----------|------:|-------:
e_error_code    | OUTCOME_TRY        | Disabled |    2% |    2261
e_error_code    | OUTCOME_TRY        | Enabled  |    2% |    1699
e_error_code    | Handle some errors | Disabled |    2% |    2333
e_error_code    | Handle some errors | Enabled  |    2% |    1738
e_error_code    | OUTCOME_TRY        | Disabled |   50% |    3754
e_error_code    | OUTCOME_TRY        | Enabled  |   50% |    2466
e_error_code    | Handle some errors | Disabled |   50% |    3004
e_error_code    | Handle some errors | Enabled  |   50% |    2303
e_error_code    | OUTCOME_TRY        | Disabled |   98% |    4745
e_error_code    | OUTCOME_TRY        | Enabled  |   98% |    2897
e_error_code    | Handle some errors | Disabled |   98% |    3482
e_error_code    | Handle some errors | Enabled  |   98% |    2663
e_system_error  | OUTCOME_TRY        | Disabled |    2% |    2591
e_system_error  | OUTCOME_TRY        | Enabled  |    2% |    2053
e_system_error  | Handle some errors | Disabled |    2% |    2591
e_system_error  | Handle some errors | Enabled  |    2% |    1969
e_system_error  | OUTCOME_TRY        | Disabled |   50% |    4793
e_system_error  | OUTCOME_TRY        | Enabled  |   50% |    3375
e_system_error  | Handle some errors | Disabled |   50% |    3662
e_system_error  | Handle some errors | Enabled  |   50% |    2883
e_system_error  | OUTCOME_TRY        | Disabled |   98% |    6653
e_system_error  | OUTCOME_TRY        | Enabled  |   98% |    4602
e_system_error  | Handle some errors | Disabled |   98% |    4550
e_system_error  | Handle some errors | Enabled  |   98% |    3711
e_heavy_payload | OUTCOME_TRY        | Disabled |    2% |   31509
e_heavy_payload | OUTCOME_TRY        | Enabled  |    2% |   17713
e_heavy_payload | Handle some errors | Disabled |    2% |   28230
e_heavy_payload | Handle some errors | Enabled  |    2% |   17252
e_heavy_payload | OUTCOME_TRY        | Disabled |   50% |   44864
e_heavy_payload | OUTCOME_TRY        | Enabled  |   50% |   25929
e_heavy_payload | Handle some errors | Disabled |   50% |   43575
e_heavy_payload | Handle some errors | Enabled  |   50% |   26238
e_heavy_payload | OUTCOME_TRY        | Disabled |   98% |   63491
e_heavy_payload | OUTCOME_TRY        | Enabled  |   98% |   35355
e_heavy_payload | Handle some errors | Disabled |   98% |   61875
e_heavy_payload | Handle some errors | Enabled  |   98% |   34624

## Charts

The charts below are generated from the results from the previous section, converted from elapsed time in microseconds to millions of calls per second (so, bigger is better). Outcome performance is plotted in grey, LEAF performance is plotted in purple for clang, orange for gcc.

| Error rate | `e_error_code` (clang) | `e_system_error` (clang) | `e_heavy_payload` (clang) |
|:---:|:---:|:---:|:---:|
| 2% | ![](clang_e_error_code_2.png) | ![](clang_e_system_error_2.png) | ![](clang_e_heavy_payload_2.png) |
| 50% | ![](clang_e_error_code_50.png) | ![](clang_e_system_error_50.png) | ![](clang_e_heavy_payload_50.png) |
| 98% | ![](clang_e_error_code_98.png) | ![](clang_e_system_error_98.png) | ![](clang_e_heavy_payload_98.png) |

| Error rate | `e_error_code` (gcc) | `e_system_error` (gcc) | `e_heavy_payload` (gcc) |
|:---:|:---:|:---:|:---:|
| 2% | ![](gcc_e_error_code_2.png) | ![](gcc_e_system_error_2.png) | ![](gcc_e_heavy_payload_2.png) |
| 50% | ![](gcc_e_error_code_50.png) | ![](gcc_e_system_error_50.png) | ![](gcc_e_heavy_payload_50.png) |
| 98% | ![](gcc_e_error_code_98.png) | ![](gcc_e_system_error_98.png) | ![](gcc_e_heavy_payload_98.png) |
