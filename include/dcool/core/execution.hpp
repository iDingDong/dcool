#ifndef DCOOL_CORE_EXECUTION_HPP_INCLUDED_
#	define DCOOL_CORE_EXECUTION_HPP_INCLUDED_ 1

#	include <dcool/core/concept.hpp>

#	include <execution>

namespace dcool::core {
	struct SerialExecution {
	};

	using SequencedExecution = ::std::execution::sequenced_policy;
	using ParallelExecution = ::std::execution::parallel_policy;
	using VectorizedExecution = ::std::execution::unsequenced_policy;
	using ParallelVectorizedExecution = ::std::execution::parallel_unsequenced_policy;

	constexpr SerialExecution serialExecution;
	constexpr SequencedExecution sequencedExecution = ::std::execution::seq;
	constexpr ParallelExecution parallelExecution = ::std::execution::par;
	constexpr VectorizedExecution vectorizedExecution = ::std::execution::unseq;
	constexpr ParallelVectorizedExecution parallelVectorizedExecution = ::std::execution::par_unseq;

	template <typename T_> concept StandardExecutionPolicy = ::std::is_execution_policy_v<T_>;

	template <typename T_> concept FormOfStandardExecutionPolicy = ::dcool::core::StandardExecutionPolicy<
		::dcool::core::QualifiedReferenceRemovedType<T_>
	>;

	template <typename T_> concept ExtentedExecutionPolicy = ::dcool::core::OneOf<T_, ::dcool::core::SerialExecution>;

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
}

#endif
