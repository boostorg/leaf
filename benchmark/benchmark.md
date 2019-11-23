# Benchmark

The LEAF github repository contains two similar benchmarking programs, one using LEAF, the other using Boost Outcome, that simulate transporting error objects across 32 levels of function calls, measuring the performance of the two libraries.

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

```
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

## Benchmark matrix dimensions

The benchmark matrix has 4 dimensions:

1. Check only vs. handle some errors:

	a. Each function calls the lower level function, does work on success, forwards errors to the caller, but does not handle them (the root of the call chain handles the error).

	b. Each fourth level of function call doesn't only check, but also handles some errors, forwarding other errors to the caller.

2. Error object type:

	a. The error object transported in case of a failure is of type `e_error_code`, which is a simple `enum`.

	b. The error object transported in case of a failure is of type `e_heavy_payload`, a `struct` of size 4096.

3. Error rate: 2%, 50%, 98%

4. Inline vs. noinline (32 levels of function calls).

## Results

Below is the output the benchmark programs running on a MacBook Pro. In addition, the programs generate a `benchmark.csv` file in the current working directory.

### clang:

```
1000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096
LEAF
                |                    | Function | Error | Elapsed
Error type      | At each level      | inlining | rate  |    (μs)
----------------|--------------------|----------|-------|--------
e_error_code    | LEAF_AUTO          | Disabled |    2% |     111
e_error_code    | LEAF_AUTO          | Enabled  |    2% |      10
e_error_code    | try_handle_some    | Disabled |    2% |     157
e_error_code    | try_handle_some    | Enabled  |    2% |      80
e_error_code    | LEAF_AUTO          | Disabled |   50% |     111
e_error_code    | LEAF_AUTO          | Enabled  |   50% |      22
e_error_code    | try_handle_some    | Disabled |   50% |     174
e_error_code    | try_handle_some    | Enabled  |   50% |     105
e_error_code    | LEAF_AUTO          | Disabled |   98% |     100
e_error_code    | LEAF_AUTO          | Enabled  |   98% |      21
e_error_code    | try_handle_some    | Disabled |   98% |     180
e_error_code    | try_handle_some    | Enabled  |   98% |     111
----------------|--------------------|----------|-------|--------
e_heavy_payload | LEAF_AUTO          | Disabled |    2% |     128
e_heavy_payload | LEAF_AUTO          | Enabled  |    2% |      12
e_heavy_payload | try_handle_some    | Disabled |    2% |     164
e_heavy_payload | try_handle_some    | Enabled  |    2% |      83
e_heavy_payload | LEAF_AUTO          | Disabled |   50% |     177
e_heavy_payload | LEAF_AUTO          | Enabled  |   50% |      64
e_heavy_payload | try_handle_some    | Disabled |   50% |     224
e_heavy_payload | try_handle_some    | Enabled  |   50% |     143
e_heavy_payload | LEAF_AUTO          | Disabled |   98% |     208
e_heavy_payload | LEAF_AUTO          | Enabled  |   98% |     103
e_heavy_payload | try_handle_some    | Disabled |   98% |     268
e_heavy_payload | try_handle_some    | Enabled  |   98% |     193
```

```
1000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096
Outcome
                |                    | Function | Error | Elapsed
Error type      | At each level      | inlining | rate  |    (μs)
----------------|--------------------|----------|-------|--------
e_error_code    | OUTCOME_TRY        | Disabled |    2% |     232
e_error_code    | OUTCOME_TRY        | Enabled  |    2% |     221
e_error_code    | Handle some errors | Disabled |    2% |     233
e_error_code    | Handle some errors | Enabled  |    2% |     233
e_error_code    | OUTCOME_TRY        | Disabled |   50% |     439
e_error_code    | OUTCOME_TRY        | Enabled  |   50% |     436
e_error_code    | Handle some errors | Disabled |   50% |     349
e_error_code    | Handle some errors | Enabled  |   50% |     373
e_error_code    | OUTCOME_TRY        | Disabled |   98% |     600
e_error_code    | OUTCOME_TRY        | Enabled  |   98% |     628
e_error_code    | Handle some errors | Disabled |   98% |     439
e_error_code    | Handle some errors | Enabled  |   98% |     443
----------------|--------------------|----------|-------|--------
e_heavy_payload | OUTCOME_TRY        | Disabled |    2% |    3749
e_heavy_payload | OUTCOME_TRY        | Enabled  |    2% |    3483
e_heavy_payload | Handle some errors | Disabled |    2% |    3489
e_heavy_payload | Handle some errors | Enabled  |    2% |    3298
e_heavy_payload | OUTCOME_TRY        | Disabled |   50% |    5179
e_heavy_payload | OUTCOME_TRY        | Enabled  |   50% |    5190
e_heavy_payload | Handle some errors | Disabled |   50% |    5221
e_heavy_payload | Handle some errors | Enabled  |   50% |    5174
e_heavy_payload | OUTCOME_TRY        | Disabled |   98% |    6749
e_heavy_payload | OUTCOME_TRY        | Enabled  |   98% |    6465
e_heavy_payload | Handle some errors | Disabled |   98% |    7134
e_heavy_payload | Handle some errors | Enabled  |   98% |    6770
```

### gcc:

```
1000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096
LEAF
                |                    | Function | Error | Elapsed
Error type      | At each level      | inlining | rate  |    (μs)
----------------|--------------------|----------|-------|--------
e_error_code    | LEAF_AUTO          | Disabled |    2% |     278
e_error_code    | LEAF_AUTO          | Enabled  |    2% |       8
e_error_code    | try_handle_some    | Disabled |    2% |     296
e_error_code    | try_handle_some    | Enabled  |    2% |      45
e_error_code    | LEAF_AUTO          | Disabled |   50% |     182
e_error_code    | LEAF_AUTO          | Enabled  |   50% |      22
e_error_code    | try_handle_some    | Disabled |   50% |     290
e_error_code    | try_handle_some    | Enabled  |   50% |      70
e_error_code    | LEAF_AUTO          | Disabled |   98% |     145
e_error_code    | LEAF_AUTO          | Enabled  |   98% |      25
e_error_code    | try_handle_some    | Disabled |   98% |     257
e_error_code    | try_handle_some    | Enabled  |   98% |      73
----------------|--------------------|----------|-------|--------
e_heavy_payload | LEAF_AUTO          | Disabled |    2% |     217
e_heavy_payload | LEAF_AUTO          | Enabled  |    2% |      11
e_heavy_payload | try_handle_some    | Disabled |    2% |     271
e_heavy_payload | try_handle_some    | Enabled  |    2% |      52
e_heavy_payload | LEAF_AUTO          | Disabled |   50% |     227
e_heavy_payload | LEAF_AUTO          | Enabled  |   50% |      90
e_heavy_payload | try_handle_some    | Disabled |   50% |     302
e_heavy_payload | try_handle_some    | Enabled  |   50% |     131
e_heavy_payload | LEAF_AUTO          | Disabled |   98% |     224
e_heavy_payload | LEAF_AUTO          | Enabled  |   98% |     156
e_heavy_payload | try_handle_some    | Disabled |   98% |     308
e_heavy_payload | try_handle_some    | Enabled  |   98% |     204
```

```
1000 iterations, call depth 32, sizeof(e_heavy_payload) = 4096
Outcome
                |                    | Function | Error | Elapsed
Error type      | At each level      | inlining | rate  |    (μs)
----------------|--------------------|----------|-------|--------
e_error_code    | OUTCOME_TRY        | Disabled |    2% |     244
e_error_code    | OUTCOME_TRY        | Enabled  |    2% |     180
e_error_code    | Handle some errors | Disabled |    2% |     271
e_error_code    | Handle some errors | Enabled  |    2% |     188
e_error_code    | OUTCOME_TRY        | Disabled |   50% |     337
e_error_code    | OUTCOME_TRY        | Enabled  |   50% |     265
e_error_code    | Handle some errors | Disabled |   50% |     316
e_error_code    | Handle some errors | Enabled  |   50% |     245
e_error_code    | OUTCOME_TRY        | Disabled |   98% |     427
e_error_code    | OUTCOME_TRY        | Enabled  |   98% |     344
e_error_code    | Handle some errors | Disabled |   98% |     347
e_error_code    | Handle some errors | Enabled  |   98% |     284
----------------|--------------------|----------|-------|--------
e_heavy_payload | OUTCOME_TRY        | Disabled |    2% |    3860
e_heavy_payload | OUTCOME_TRY        | Enabled  |    2% |    2020
e_heavy_payload | Handle some errors | Disabled |    2% |    3589
e_heavy_payload | Handle some errors | Enabled  |    2% |    2373
e_heavy_payload | OUTCOME_TRY        | Disabled |   50% |    6070
e_heavy_payload | OUTCOME_TRY        | Enabled  |   50% |    3091
e_heavy_payload | Handle some errors | Disabled |   50% |    5222
e_heavy_payload | Handle some errors | Enabled  |   50% |    3460
e_heavy_payload | OUTCOME_TRY        | Disabled |   98% |    8210
e_heavy_payload | OUTCOME_TRY        | Enabled  |   98% |    4199
e_heavy_payload | Handle some errors | Disabled |   98% |    8096
e_heavy_payload | Handle some errors | Enabled  |   98% |    4690
```

## Charts

The charts below are generated from the results from the previous section, converted from microseconds to millions of calls per seconds.

| Error rate | `e_error_code` (clang) | `e_heavy_payload` (clang) |
|:---:|:---:|:---:|
| 2% | ![](clang_e_error_code_2.png) | ![](clang_e_heavy_payload_2.png) |
| 50% | ![](clang_e_error_code_50.png) | ![](clang_e_heavy_payload_50.png) |
| 98% | ![](clang_e_error_code_98.png) | ![](clang_e_heavy_payload_98.png) |

| Error rate | `e_error_code` (gcc) | `e_heavy_payload` (gcc) |
|:---:|:---:|:---:|
| 2% | ![](gcc_e_error_code_2.png) | ![](gcc_e_heavy_payload_2.png) |
| 50% | ![](gcc_e_error_code_50.png) | ![](gcc_e_heavy_payload_50.png) |
| 98% | ![](gcc_e_error_code_98.png) | ![](gcc_e_heavy_payload_98.png) |
