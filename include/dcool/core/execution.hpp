#ifndef DCOOL_CORE_EXECUTION_HPP_INCLUDED_
#	define DCOOL_CORE_EXECUTION_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>

#	include <exception>
#	if DCOOLER_CPP_P1135R6_ENABLED
#		include <execution>
#	endif

namespace dcool::core {
	struct SerialExecution {
	};

#	if DCOOLER_CPP_P1135R6_ENABLED
	using SequencedExecution = ::std::execution::sequenced_policy;
	using ParallelExecution = ::std::execution::parallel_policy;
	using VectorizedExecution = ::std::execution::unsequenced_policy;
	using ParallelVectorizedExecution = ::std::execution::parallel_unsequenced_policy;

	constexpr ::dcool::core::SerialExecution serialExecution;
	constexpr ::dcool::core::SequencedExecution sequencedExecution = ::std::execution::seq;
	constexpr ::dcool::core::ParallelExecution parallelExecution = ::std::execution::par;
	constexpr ::dcool::core::VectorizedExecution vectorizedExecution = ::std::execution::unseq;
	constexpr ::dcool::core::ParallelVectorizedExecution parallelVectorizedExecution = ::std::execution::par_unseq;

	template <typename T_> concept StandardExecutionPolicy = ::std::is_execution_policy_v<T_>;

	template <typename T_> concept ExtentedExecutionPolicy = ::dcool::core::OneOf<T_, ::dcool::core::SerialExecution>;
#	else
	struct SequencedExecution {
	};

	struct ParallelExecution {
	};

	struct VectorizedExecution {
	};

	struct ParallelVectorizedExecution {
	};

	constexpr ::dcool::core::SerialExecution serialExecution;
	constexpr ::dcool::core::SequencedExecution sequencedExecution;
	constexpr ::dcool::core::ParallelExecution parallelExecution;
	constexpr ::dcool::core::VectorizedExecution vectorizedExecution;
	constexpr ::dcool::core::ParallelVectorizedExecution parallelVectorizedExecution;

	template <typename T_> concept StandardExecutionPolicy = false;

	template <typename T_> concept ExtentedExecutionPolicy = ::dcool::core::OneOf<
		T_,
		::dcool::core::SerialExecution,
		::dcool::core::SequencedExecution,
		::dcool::core::ParallelExecution,
		::dcool::core::VectorizedExecution,
		::dcool::core::ParallelVectorizedExecution
	>;
#	endif

	template <typename T_> concept FormOfStandardExecutionPolicy = ::dcool::core::StandardExecutionPolicy<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>;

	template <typename T_> concept FormOfExtentedExecutionPolicy = ::dcool::core::ExtentedExecutionPolicy<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>;

	template <typename T_> concept ExecutionPolicy =
		::dcool::core::FormOfExtentedExecutionPolicy<T_> || ::dcool::core::StandardExecutionPolicy<T_>
	;

	template <typename T_> concept FormOfExecutionPolicy = ::dcool::core::ExecutionPolicy<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>;

	template <typename T_> concept SequencedPolicy = ::dcool::core::OneOf<
		T_, ::dcool::core::SerialExecution, ::dcool::core::SequencedExecution
	>;

	template <typename T_> concept FormOfSequencedPolicy = ::dcool::core::SequencedPolicy<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>;

	template <
		typename ReturnedT_, typename... ArgumentTs_
	> [[noreturn]] auto terminate(ArgumentTs_&&...) noexcept -> ReturnedT_ {
		::std::terminate();
	}

	template <
		typename... ArgumentTs_
	> [[noreturn]] void terminate(ArgumentTs_&&... parameters_) noexcept {
		::dcool::core::terminate<void>(::dcool::core::forward<ArgumentTs_>(parameters_)...);
	}
}

#endif
