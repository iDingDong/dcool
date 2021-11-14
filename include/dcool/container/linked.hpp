#ifndef DCOOL_CONTAINER_LINKED_HPP_INCLUDED_
#	define DCOOL_CONTAINER_LINKED_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::container::detail_, HasTypeLinkedNodeForLinked_, ExtractedLinkedNodeForLinkedType_, LinkedNode
)

namespace dcool::container {
	namespace detail_ {
		template <typename NodeHeaderT_> using LinkedNodeHeaderHandleType_ =
			::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<NodeHeaderT_&>().nextHandle())>
		;

		template <typename NodeT_> using LinkedNodeHandleType_ = ::dcool::container::detail_::LinkedNodeHeaderHandleType_<
			::dcool::core::NodeHeaderType<NodeT_>
		>;
	}

	template <
		typename T_,
		typename HandleConverterT_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
			::dcool::container::detail_::LinkedNodeHeaderHandleType_<T_>, void*
		>
	> concept ForwardLinkedNodeHeader = ::dcool::core::BidirectionalConsistentConverter<
		HandleConverterT_, ::dcool::container::detail_::LinkedNodeHeaderHandleType_<T_>, void*
	> && requires (T_ nodeHeader_, ::dcool::container::detail_::LinkedNodeHeaderHandleType_<T_> handle_) {
		handle_ = nodeHeader_.nextHandle();
		nodeHeader_.setNextHandle(handle_);
	};

	template <
		typename T_,
		typename HandleConverterT_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
			::dcool::container::detail_::LinkedNodeHeaderHandleType_<T_>, void*
		>
	> concept BidirectionalLinkedNodeHeader = ::dcool::container::ForwardLinkedNodeHeader<T_, HandleConverterT_> && requires (
		T_ nodeHeader_, ::dcool::container::detail_::LinkedNodeHeaderHandleType_<T_> handle_
	) {
		handle_ = nodeHeader_.previousHandle();
		nodeHeader_.setPreviousHandle(handle_);
	};

	template <typename T_, typename HandleConverterT_> concept ForwardLinkedNode =
		::dcool::core::ValueNode<T_> &&
		::dcool::container::ForwardLinkedNodeHeader<::dcool::core::NodeHeaderType<T_>, HandleConverterT_>
	;

	template <typename T_, typename HandleConverterT_> concept BidirectionalLinkedNode =
		::dcool::core::ValueNode<T_> &&
		::dcool::container::BidirectionalLinkedNodeHeader<::dcool::core::NodeHeaderType<T_>, HandleConverterT_>
	;

	namespace detail_ {
		template <typename NodeHeaderT_, typename ConverterT_> constexpr auto nextLinkedNodeHeader_(
			NodeHeaderT_& leading_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			return *reinterpret_cast<NodeHeaderT_*>(converter_(leading_.nextHandle()));
		}

		template <typename NodeHeaderT_, typename ConverterT_> requires (
			::dcool::container::BidirectionalLinkedNodeHeader<NodeHeaderT_, ConverterT_>
		) constexpr auto previousLinkedNodeHeader_(
			NodeHeaderT_& tailing_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			return *reinterpret_cast<NodeHeaderT_*>(converter_(tailing_.previousHandle()));
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr void connectLinkedNodeHeader_(
			NodeHeaderT_& leading_, NodeHeaderT_& following_, ConverterT_ const& converter_
		) noexcept {
			leading_.setNextHandle(converter_(static_cast<void*>(::dcool::core::addressOf(following_))));
			if constexpr (::dcool::container::BidirectionalLinkedNodeHeader<NodeHeaderT_, ConverterT_>) {
				following_.setPreviousHandle(converter_(static_cast<void*>(::dcool::core::addressOf(leading_))));
			}
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr void insertLinkedNodeHeaderAfter_(
			NodeHeaderT_& leading_, NodeHeaderT_& toInsert_, ConverterT_ const& converter_
		) noexcept {
			NodeHeaderT_& tailing_ = ::dcool::container::detail_::nextLinkedNodeHeader_(leading_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(toInsert_, tailing_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(leading_, toInsert_, converter_);
		}

		template <typename NodeHeaderT_, typename ConverterT_> requires (
			::dcool::container::BidirectionalLinkedNodeHeader<NodeHeaderT_, ConverterT_>
		) constexpr void insertLinkedNodeHeaderBefore_(
			NodeHeaderT_& tailing_, NodeHeaderT_& toInsert_, ConverterT_ const& converter_
		) noexcept {
			NodeHeaderT_& leading_ = ::dcool::container::detail_::previousLinkedNodeHeader_(tailing_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(toInsert_, tailing_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(leading_, toInsert_, converter_);
		}

		template <typename NodeHeaderT_, typename ConverterT_> requires (
			::dcool::container::BidirectionalLinkedNodeHeader<NodeHeaderT_, ConverterT_>
		) constexpr auto popLinkedNodeHeader_(
			NodeHeaderT_& toPop_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			NodeHeaderT_& leading_ = ::dcool::container::detail_::previousLinkedNodeHeader_(toPop_, converter_);
			NodeHeaderT_& tailing_ = ::dcool::container::detail_::nextLinkedNodeHeader_(toPop_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(leading_, tailing_, converter_);
			return toPop_;
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr auto popLinkedNodeHeaderAfter_(
			NodeHeaderT_& leading_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			NodeHeaderT_& result_ = ::dcool::container::detail_::nextLinkedNodeHeader_(leading_, converter_);
			NodeHeaderT_& tailing_ = ::dcool::container::detail_::nextLinkedNodeHeader_(result_, converter_);
			::dcool::container::detail_::connectLinkedNodeHeader_(leading_, tailing_, converter_);
			return result_;
		}

		// The distiguisher is intended to prevent node or iterator from being convertible between different type of linked.
		template <typename HandleT_, typename DistinguisherT_ = void> struct DefaultForwardLinkedNodeHeader_ {
			public: using Handle = HandleT_;
			private: HandleT_ m_next_;

			public: constexpr auto nextHandle() const noexcept -> Handle {
				return this->m_next_;
			}

			public: constexpr auto setNextHandle(Handle handle_) noexcept {
				return this->m_next_ = handle_;
			}
		};

		// The distiguisher is intended to prevent node or iterator from being convertible between different type of linked.
		template <
			typename HandleT_, typename DistinguisherT_ = void
		> struct DefaultBidirectionalLinkedNodeHeader_: private ::dcool::container::detail_::DefaultForwardLinkedNodeHeader_<
			HandleT_, DistinguisherT_
		> {
			private: using Super_ = ::dcool::container::detail_::DefaultForwardLinkedNodeHeader_<HandleT_, DistinguisherT_>;
			public: using Handle = HandleT_;

			private: HandleT_ m_previous_;

			public: using Super_::nextHandle;
			public: using Super_::setNextHandle;

			public: constexpr auto previousHandle() const noexcept -> Handle {
				return this->m_previous_;
			}

			public: constexpr auto setPreviousHandle(Handle handle_) noexcept {
				return this->m_previous_ = handle_;
			}
		};

		template <
			typename HandleT_, typename ValueT_, typename DistinguisherT_ = void
		> using DefaultForwardLinkedNode_ = ::dcool::core::DefaultValueNode<
			ValueT_, ::dcool::container::detail_::DefaultForwardLinkedNodeHeader_<HandleT_, DistinguisherT_>
		>;

		template <
			typename HandleT_, typename ValueT_, typename DistinguisherT_ = void
		> using DefaultBidirectionalLinkedNode_ = ::dcool::core::DefaultValueNode<
			ValueT_, ::dcool::container::detail_::DefaultBidirectionalLinkedNodeHeader_<HandleT_, DistinguisherT_>
		>;

		template <typename ValueT_> struct HandleConverterOfEngineForHelper_ {
			template <typename EngineT__> static constexpr auto get_(EngineT__& engine_) noexcept {
				return ::dcool::resource::adaptedHandleConverterFor<ValueT_>(engine_.pool());
			}
		};

		template <typename ValueT_> struct HandleConverterOfEngineForHelper_<ValueT_ const> {
			template <typename EngineT__> static constexpr auto get_(EngineT__& engine_) noexcept {
				return ::dcool::resource::adaptedConstHandleConverterFor<ValueT_>(engine_.pool());
			}
		};

		template <typename EngineT_, typename ValueT_> struct UnifiedHandleOf_ {
			using Result_ = ::dcool::resource::PoolAdaptorFor<
				::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<EngineT_&>().pool())>, ValueT_
			>::UnifiedHandle;
		};

		template <typename EngineT_, typename ValueT_> struct UnifiedHandleOf_<EngineT_, ValueT_ const> {
			using Result_ = ::dcool::resource::PoolAdaptorFor<
				::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<EngineT_&>().pool())>, ValueT_
			>::UnifiedConstHandle;
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeHeaderT_, ::dcool::core::Boolean reversedC_
		> struct LinkedHeaderIteratorBase_ {
			private: using Self_ = LinkedHeaderIteratorBase_<HandleConverterT_, EngineT_, NodeHeaderT_, reversedC_>;
			public: using HandleConverter = HandleConverterT_;
			public: using Engine = EngineT_;
			public: using NodeHeader = NodeHeaderT_;

			public: using Handle = ::dcool::container::detail_::UnifiedHandleOf_<EngineT_, NodeHeaderT_>::Result_;
			public: static constexpr ::dcool::core::Boolean bidirectional = ::dcool::container::BidirectionalLinkedNodeHeader<
				NodeHeaderT_, HandleConverterT_
			>;
			static_assert(bidirectional || (!reversedC_));

			private: Handle m_handle_;

			public: LinkedHeaderIteratorBase_() = default;
			public: constexpr LinkedHeaderIteratorBase_(Self_ const&) noexcept = default;
			public: constexpr LinkedHeaderIteratorBase_(Self_&&) noexcept = default;

			public: constexpr explicit LinkedHeaderIteratorBase_(Handle handle_) noexcept: m_handle_(handle_) {
			}

			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: constexpr auto nodeHeaderHandle() const noexcept -> Handle {
				return this->m_handle_;
			}

			public: constexpr auto nodeHeader(HandleConverter const& converter_) const noexcept -> NodeHeader& {
				return *::dcool::core::launder(static_cast<NodeHeader*>(converter_(this->nodeHeaderHandle())));
			}

			public: constexpr auto nodeHeader(Engine& engine_) const noexcept -> NodeHeader& {
				HandleConverter converter_ = ::dcool::container::detail_::HandleConverterOfEngineForHelper_<NodeHeader>::get_(engine_);
				return this->nodeHeader(converter_);
			}

			public: constexpr auto nextHandle(HandleConverter const& converter_) const noexcept -> Handle {
				if constexpr (reversedC_) {
					return this->nodeHeader(converter_).previousHandle();
				}
				return this->nodeHeader(converter_).nextHandle();
			}

			public: constexpr auto nextHandle(Engine& engine_) const noexcept -> Handle {
				if constexpr (reversedC_) {
					return this->nodeHeader(engine_).previousHandle();
				}
				return this->nodeHeader(engine_).nextHandle();
			}

			public: constexpr auto previousHandle(
				HandleConverter const& converter_
			) const noexcept -> Handle requires (bidirectional) {
				if constexpr (reversedC_) {
					return this->nodeHeader(converter_).nextHandle();
				}
				return this->nodeHeader(converter_).previousHandle();
			}

			public: constexpr auto previousHandle(Engine& engine_) const noexcept -> Handle requires (bidirectional) {
				if constexpr (reversedC_) {
					return this->nodeHeader(engine_).nextHandle();
				}
				return this->nodeHeader(engine_).previousHandle();
			}

			public: constexpr void advance(HandleConverter const& converter_) noexcept {
				this->m_handle_ = this->nextHandle(converter_);
			}

			public: constexpr void advance(Engine& engine_) noexcept {
				this->m_handle_ = this->nextHandle(engine_);
			}

			public: constexpr void retreat(HandleConverter const& converter_) noexcept requires (bidirectional) {
				this->m_handle_ = this->previousHandle(converter_);
			}

			public: constexpr void retreat(Engine& engine_) noexcept requires (bidirectional) {
				this->m_handle_ = this->previousHandle(engine_);
			}

			public: constexpr auto next(HandleConverter const& converter_) const noexcept -> Self_ {
				return Self_(this->nextHandle(converter_));
			}

			public: constexpr auto next(Engine& engine_) const noexcept -> Self_ {
				return Self_(this->nextHandle(engine_));
			}

			public: constexpr auto previous(HandleConverter const& converter_) const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->previousHandle(converter_));
			}

			public: constexpr auto previous(Engine& engine_) const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->previousHandle(engine_));
			}

			public: constexpr auto equalsTo(
				Self_ const& other_, HandleConverter const& converter_
			) const noexcept -> ::dcool::core::Boolean {
				return
					::dcool::core::addressOf(this->nodeHeader(converter_)) == ::dcool::core::addressOf(other_.nodeHeader(converter_))
				;
			}

			public: constexpr auto equalsTo(Self_ const& other_, Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				return ::dcool::core::addressOf(this->nodeHeader(engine_)) == ::dcool::core::addressOf(other_.nodeHeader(engine_));
			}

			public: static constexpr auto equal(
				Self_ const& first_, Self_ const& second_, HandleConverter const& converter_
			) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_, converter_);
			}

			public: static constexpr auto equal(
				Self_ const& first_, Self_ const& second_, Engine& engine_
			) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_, engine_);
			}
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeT_, ::dcool::core::Boolean reversedC_
		> struct LinkedIterator_;

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeT_, ::dcool::core::Boolean reversedC_
		> struct LinkedLightIterator_: private ::dcool::container::detail_::LinkedHeaderIteratorBase_<
			HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>, reversedC_
		> {
			private: using Self_ = LinkedLightIterator_<HandleConverterT_, EngineT_, NodeT_, reversedC_>;
			private: using Super_ = ::dcool::container::detail_::LinkedHeaderIteratorBase_<
				HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>, reversedC_
			>;
			public: using Node = NodeT_;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;

			public: using Value = ::dcool::core::NodeValueType<Node>;
			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::Handle;
			public: using Super_::bidirectional;
			public: using Super_::nodeHeaderHandle;
			public: using Super_::nodeHeader;
			public: using Super_::nextHandle;
			public: using Super_::previousHandle;
			public: using Super_::advance;
			public: using Independent = ::dcool::container::detail_::LinkedIterator_<HandleConverter, Engine, Node, reversedC_>;

			public: LinkedLightIterator_() = default;
			public: constexpr LinkedLightIterator_(Self_ const&) noexcept = default;
			public: constexpr LinkedLightIterator_(Self_&&) noexcept = default;

			public: constexpr explicit LinkedLightIterator_(Handle handle_) noexcept: Super_(handle_) {
			}

			private: template <
				::dcool::core::FormOfSame<Super_> SuperT__
			> constexpr explicit LinkedLightIterator_(SuperT__&& super_) noexcept: Super_(
				::dcool::core::forward<SuperT__>(super_)
			) {
			}

			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: constexpr auto node(HandleConverter const& converter_) const noexcept -> Node& {
				return Node::retrieveFromHeader(this->nodeHeader(converter_));
			}

			public: constexpr auto node(Engine& engine_) const noexcept -> Node& {
				return Node::retrieveFromHeader(this->nodeHeader(engine_));
			}

			public: constexpr auto next(HandleConverter const& converter_) const noexcept -> Self_ {
				return Self_(this->Super_::next(converter_));
			}

			public: constexpr auto next(Engine& engine_) const noexcept -> Self_ {
				return Self_(this->Super_::next(engine_));
			}

			public: constexpr auto previous(HandleConverter const& converter_) const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->Super_::previous(converter_));
			}

			public: constexpr auto previous(Engine& engine_) const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->Super_::previous(engine_));
			}

			public: constexpr auto dereferenceSelf(HandleConverter const& converter_) const noexcept {
				return this->node(converter_).value();
			}

			public: constexpr auto dereferenceSelf(Engine& engine_) const noexcept {
				return this->node(engine_).value();
			}

			public: constexpr auto equalsTo(
				Self_ const& other_, HandleConverter const& converter_
			) const noexcept -> ::dcool::core::Boolean {
				return this->Super_::equalsTo(static_cast<Super_>(other_), converter_);
			}

			public: constexpr auto equalsTo(Self_ const& other_, Engine& engine_) const noexcept -> ::dcool::core::Boolean {
				return this->Super_::equalsTo(static_cast<Super_>(other_), engine_);
			}

			public: static constexpr auto equal(
				Self_ const& first_, Self_ const& second_, HandleConverter const& converter_
			) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_, converter_);
			}

			public: static constexpr auto equal(
				Self_ const& first_, Self_ const& second_, Engine& engine_
			) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_, engine_);
			}
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeHeaderT_, ::dcool::core::Boolean reversedC_
		> struct LinkedHeaderIterator_: private ::dcool::container::detail_::LinkedHeaderIteratorBase_<
			HandleConverterT_, EngineT_, NodeHeaderT_, reversedC_
		> {
			private: using Self_ = LinkedHeaderIterator_<HandleConverterT_, EngineT_, NodeHeaderT_, reversedC_>;
			private: using Super_ = ::dcool::container::detail_::LinkedHeaderIteratorBase_<
				HandleConverterT_, EngineT_, NodeHeaderT_, reversedC_
			>;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;
			private: using NodeHeader_ = NodeHeaderT_;

			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::NodeHeader;
			public: using typename Super_::Handle;
			public: using Super_::bidirectional;

			private: [[no_unique_address]] HandleConverter m_converter_;

			public: LinkedHeaderIterator_() = default;
			public: constexpr LinkedHeaderIterator_(Self_ const&) noexcept = default;
			public: constexpr LinkedHeaderIterator_(Self_&&) noexcept = default;

			public: template <::dcool::core::FormOfSame<HandleConverter> ConverterT__> constexpr LinkedHeaderIterator_(
				Handle handle_, ConverterT__&& converter_
			) noexcept: Super_(handle_), m_converter_(::dcool::core::forward<ConverterT__>(converter_)) {
			}

			private: template <
				::dcool::core::FormOfSame<Super_> SuperT__, ::dcool::core::FormOfSame<HandleConverter> ConverterT__
			> constexpr LinkedHeaderIterator_(
				SuperT__&& super_, ConverterT__&& converter_
			) noexcept:
				Super_(::dcool::core::forward<SuperT__>(super_)), m_converter_(::dcool::core::forward<ConverterT__>(converter_))
			{
			}

			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: using Super_::nodeHeaderHandle;

			public: constexpr auto converter() const noexcept -> HandleConverter const& {
				return this->m_converter_;
			}

			public: constexpr auto nodeHeader() const noexcept -> NodeHeader& {
				return this->Super_::nodeHeader(this->converter());
			}

			public: constexpr auto nextHandle() const noexcept -> Handle {
				return this->Super_::nextHandle(this->converter());
			}

			public: constexpr auto previousHandle() const noexcept -> Handle requires (bidirectional) {
				return this->Super_::previousHandle(this->converter());
			}

			public: constexpr void advance() noexcept {
				this->Super_::advance(this->converter());
			}

			public: constexpr void retreat() noexcept requires (bidirectional) {
				this->Super_::retreat(this->converter());
			}

			public: constexpr auto next() const noexcept -> Self_ {
				return Self_(this->Super_::next(this->converter()), this->converter());
			}

			public: constexpr auto previous() const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->Super_::previous(this->converter()), this->converter());
			}

			public: constexpr auto equalsTo(Self_ const& other_) const noexcept -> ::dcool::core::Boolean {
				return this->Super_::equalsTo(static_cast<Super_>(other_), this->converter());
			}

			public: static constexpr auto equal(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_);
			}

			public: friend auto operator ==(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean {
				return equal(first_, second_);
			}

			public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeT_, ::dcool::core::Boolean reversedC_
		> struct LinkedIterator_: private ::dcool::container::detail_::LinkedHeaderIterator_<
			HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>, reversedC_
		> {
			private: using Self_ = LinkedIterator_<HandleConverterT_, EngineT_, NodeT_, reversedC_>;
			private: using Super_ = ::dcool::container::detail_::LinkedHeaderIterator_<
				HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>, reversedC_
			>;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;
			public: using Node = NodeT_;

			public: using Value = ::dcool::core::NodeValueType<Node>;
			public: using typename Super_::NodeHeader;
			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::Handle;
			public: using Super_::bidirectional;
			public: using Super_::converter;
			public: using Super_::nodeHeaderHandle;
			public: using Super_::nodeHeader;
			public: using Super_::nextHandle;
			public: using Super_::previousHandle;
			public: using Super_::advance;
			public: using Super_::retreat;
			public: using Light = ::dcool::container::detail_::LinkedLightIterator_<HandleConverter, EngineT_, Node, reversedC_>;

			// For satisfying standard requirement
			public: using value_type = Value;
			public: using difference_type = ::dcool::core::Difference;

			public: LinkedIterator_() = default;
			public: constexpr LinkedIterator_(Self_ const&) noexcept = default;
			public: constexpr LinkedIterator_(Self_&&) noexcept = default;

			public: template <::dcool::core::FormOfSame<HandleConverter> ConverterT__> constexpr LinkedIterator_(
				Handle handle_, ConverterT__&& converter_
			) noexcept: Super_(handle_, ::dcool::core::forward<ConverterT__>(converter_)) {
			}

			public: constexpr LinkedIterator_(Light const& light_, Engine& engine_) noexcept: Self_(
				light_.nodeHeaderHandle(), ::dcool::container::detail_::HandleConverterOfEngineForHelper_<NodeHeader>::get_(engine_)
			) {
			}

			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			private: template <::dcool::core::FormOfSame<Super_> SuperT__> constexpr explicit LinkedIterator_(
				SuperT__&& super_
			) noexcept: Super_(::dcool::core::forward<SuperT__>(super_)) {
			}

			public: constexpr auto node() const noexcept -> Node& {
				return Node::retrieveFromHeader(this->nodeHeader());
			}

			public: constexpr auto dereferenceSelf() const noexcept -> Value& {
				return this->node().value();
			}

			public: constexpr auto next() const noexcept -> Self_ {
				return Self_(this->Super_::next());
			}

			public: constexpr auto previous() const noexcept -> Self_ requires (bidirectional) {
				return Self_(this->Super_::previous());
			}

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->advance();
				return *this;
			}

			public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
				Self_ result_(*this);
				return *this;
			}

			public: constexpr auto operator --() noexcept -> Self_& requires (bidirectional) {
				this->retreat();
				return *this;
			}

			public: constexpr auto operator --(::dcool::core::PostDisambiguator) noexcept -> Self_ requires (bidirectional) {
				Self_ result_(*this);
				return *this;
			}

			public: constexpr auto operator *() const noexcept -> Value& {
				return this->dereferenceSelf();
			}

			public: constexpr auto operator ->() const noexcept -> Value* {
				return ::dcool::core::addressOf(this->dereferenceSelf());
			}

			public: constexpr explicit operator Light() const noexcept {
				return Light(this->nodeHeaderHandle());
			}

			public: constexpr auto equalsTo(Self_ const& other_) const noexcept -> ::dcool::core::Boolean {
				return this->Super_::equalsTo(static_cast<Super_>(other_));
			}

			public: static constexpr auto equal(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean {
				return first_.equalsTo(second_);
			}

			public: friend auto operator ==(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean {
				return equal(first_, second_);
			}

			public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		};

		template <
			typename ValueT_, typename ConfigT_
		> using ForwardLinkedNodeType_ = ::dcool::container::detail_::ExtractedLinkedNodeForLinkedType_<
			ConfigT_,
			::dcool::container::detail_::DefaultForwardLinkedNode_<
				typename ::dcool::resource::PoolType<ConfigT_>::UnifiedHandle, ValueT_, ::dcool::core::Empty<ValueT_, ConfigT_>
			>
		>;

		template <
			typename ValueT_, typename ConfigT_
		> using BidirectionalLinkedNodeType_ = ::dcool::container::detail_::ExtractedLinkedNodeForLinkedType_<
			ConfigT_,
			::dcool::container::detail_::DefaultBidirectionalLinkedNode_<
				typename ::dcool::resource::PoolType<ConfigT_>::UnifiedHandle, ValueT_, ::dcool::core::Empty<ValueT_, ConfigT_>
			>
		>;

		template <typename ConfigT_, typename ValueT_> struct LinkedConfigAdaptor_ {
			private: using Self_ = LinkedConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;

			private: struct DefaultEngine_ {
				public: [[no_unique_address]] Pool partPool;

				public: constexpr auto pool() noexcept -> Pool& {
					return this->partPool;
				}
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
			static_assert(
				::dcool::core::isSame<decltype(::dcool::core::declval<Engine>().pool()), Pool&>,
				"User-defined 'Pool' does not match return value of 'Engine::pool'"
			);

			public: using LinkedNode = ::dcool::container::detail_::ForwardLinkedNodeType_<Value, Config>;
			public: using LinkedNodeHeader = ::dcool::core::NodeHeaderType<LinkedNode>;
			public: using PoolAdaptorForLinkedNode = ::dcool::resource::PoolAdaptorFor<Pool, LinkedNode>;
			public: using PoolAdaptorForLinkedNodeHeader = ::dcool::resource::PoolAdaptorFor<Pool, LinkedNodeHeader>;
			public: using Handle = PoolAdaptorForLinkedNode::UnifiedHandle;
			public: using ConstHandle = PoolAdaptorForLinkedNode::UnifiedConstHandle;
			public: using HandleConverter = PoolAdaptorForLinkedNode::HandleConverter;
			public: using ConstHandleConverter = PoolAdaptorForLinkedNode::ConstHandleConverter;
			public: using HeaderHandleConverter = PoolAdaptorForLinkedNodeHeader::HandleConverter;
			public: using HeaderConstHandleConverter = PoolAdaptorForLinkedNodeHeader::ConstHandleConverter;
			public: using LinkedIterator = ::dcool::container::detail_::LinkedIterator_<
				HeaderHandleConverter, Engine, LinkedNode, false
			>;
			public: using LinkedConstIterator = ::dcool::container::detail_::LinkedIterator_<
				HeaderConstHandleConverter, Engine, LinkedNode const, false
			>;
			public: using LinkedLightIterator = LinkedIterator::Light;
			public: using LinkedLightConstIterator = LinkedConstIterator::Light;
		};

		template <
			typename T_, typename ValueT_, typename AdaptorHelperT_ = LinkedConfigAdaptor_<T_, ValueT_>
		> concept ForwardLinkedConfigWithAdaptorHelper_ =
			::dcool::core::Object<ValueT_> &&
			::dcool::resource::PoolFor<typename AdaptorHelperT_::Pool, typename AdaptorHelperT_::LinkedNode> &&
			::dcool::resource::PoolFor<typename AdaptorHelperT_::Pool, typename AdaptorHelperT_::LinkedNodeHeader> &&
			::dcool::core::SameAs<
				typename AdaptorHelperT_::Value, ::dcool::core::NodeValueType<typename AdaptorHelperT_::LinkedNode>
			> &&
			::dcool::core::SameAs<
				typename AdaptorHelperT_::Handle,
				::dcool::container::detail_::LinkedNodeHandleType_<typename AdaptorHelperT_::LinkedNode>
			>
		;
	}

	template <typename T_, typename ValueT_> concept LinkedConfig =
		::dcool::container::detail_::ForwardLinkedConfigWithAdaptorHelper_<T_, ValueT_>
	;

	namespace detail_ {
		template <typename BaseConfigT_, typename ValueT_> struct BidirectionalLinkedUnderlyingConfig_: public BaseConfigT_ {
			using LinkedNode = ::dcool::container::detail_::BidirectionalLinkedNodeType_<ValueT_, BaseConfigT_>;
		};

		template <typename T_, typename ValueT_> concept BidirectionalLinkedConfig_ =
			::dcool::container::LinkedConfig<T_, ValueT_> &&
			::dcool::container::BidirectionalLinkedNode<
				typename ::dcool::container::detail_::LinkedConfigAdaptor_<T_, ValueT_>::LinkedNode,
				typename ::dcool::container::detail_::LinkedConfigAdaptor_<T_, ValueT_>::HeaderHandleConverter
			>
		;
	}

	template <typename T_, typename ValueT_> concept BidirectionalLinkedConfig =
		::dcool::container::detail_::BidirectionalLinkedConfig_<
			::dcool::container::detail_::BidirectionalLinkedUnderlyingConfig_<T_, ValueT_>, ValueT_
		>
	;

	template <
		typename ConfigT_, typename ValueT_
	> requires ::dcool::container::LinkedConfig<ConfigT_, ValueT_> using LinkedConfigAdaptor =
		::dcool::container::detail_::LinkedConfigAdaptor_<ConfigT_, ValueT_>
	;

	template <
		::dcool::core::Object ValueT_, ::dcool::container::LinkedConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct LinkedChassis {
		private: using Self_ = LinkedChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::container::LinkedConfigAdaptor<Config, Value>;
		public: using Node = ConfigAdaptor_::LinkedNode;
		public: using NodeHeader = ConfigAdaptor_::LinkedNodeHeader;
		public: using Pool = ConfigAdaptor_::Pool;
		private: using PoolAdaptorForNode_ = ConfigAdaptor_::PoolAdaptorForLinkedNode;
		private: using PoolAdaptorForNodeHeader_ = ConfigAdaptor_::PoolAdaptorForLinkedNodeHeader;
		public: using ConstHandle = ConfigAdaptor_::ConstHandle;
		public: using Handle = ConfigAdaptor_::Handle;
		public: using HandleConverter = ConfigAdaptor_::HandleConverter;
		public: using HeaderHandleConverter = ConfigAdaptor_::HeaderHandleConverter;
		public: using Iterator = ConfigAdaptor_::LinkedIterator;
		public: using ConstIterator = ConfigAdaptor_::LinkedConstIterator;
		public: using LightIterator = ConfigAdaptor_::LinkedLightIterator;
		public: using ConstLightIterator = ConfigAdaptor_::LinkedLightConstIterator;
		public: using SentryHolder_ = ::dcool::resource::HandleReferenceable<NodeHeader, Pool>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean bidirectional = ::dcool::container::BidirectionalLinkedNodeHeader<
			NodeHeader, HeaderHandleConverter
		>;

		public: constexpr LinkedChassis() noexcept = default;
		public: LinkedChassis(Self_ const&) = delete;
		public: LinkedChassis(Self_&&) = delete;
		public: constexpr ~LinkedChassis() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: SentryHolder_ m_sentryHolder_;

		public: constexpr void initialize(Engine& engine_) noexcept {
			this->m_sentryHolder_.initialize(engine_.pool());
			NodeHeader& sentry_ = this->m_sentryHolder_.value(engine_.pool());
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			::dcool::container::detail_::connectLinkedNodeHeader_(sentry_, sentry_, headerConverter_);
		}

		public: constexpr void relocateTo(Engine& engine_, Self_& other_) noexcept {
			this->m_sentryHolder_.relocateTo(engine_.pool(), other_.m_sentryHolder_);
		}

		public: template <typename ValueT__, typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::container::LinkedChassis<ValueT__, ConfigT__>::Engine& otherEngine_,
			::dcool::container::LinkedChassis<ValueT__, ConfigT__>& other_
		) const {
			other_.initialize(otherEngine_);
			try {
				ConstLightIterator begin_ = this->lightBegin(engine_);
				ConstLightIterator end_ = this->lightEnd(engine_);
				typename ::dcool::container::LinkedChassis<
					ValueT__, ConfigT__
				>::LightIterator current_ = other_.lightBegin(otherEngine_);
				for (; !(ConstLightIterator::equal(begin_, end_, engine_)); begin_.advance(engine_)) {
					current_ = other_.emplaceAfter(otherEngine_, current_, begin_.dereferenceSelf(engine_));
				}
			} catch (...) {
				other_.uninitialize(otherEngine_);
				throw;
			}
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			HandleConverter converter_ = PoolAdaptorForNode_::handleConverter(engine_.pool());
			LightIterator begin_ = this->lightBegin(engine_);
			LightIterator end_ = this->lightEnd(engine_);
			while (!(begin_.equalsTo(end_, headerConverter_))) {
				Node* node_ = ::dcool::core::addressOf(begin_.node(headerConverter_));
				Handle nodeHandle_ = converter_(static_cast<void*>(node_));
				begin_.advance(headerConverter_);
				destroyNode_(engine_, nodeHandle_);
			}
			this->m_sentryHolder_.uninitialize(engine_.pool());
		}

		public: constexpr void swapWith(Engine engine_, Engine& otherEngine_, Self_& other_) noexcept {
			Self_ middleMan_;
			this->relocateTo(engine_, middleMan_);
			other_.relocateTo(otherEngine_, *this);
			middleMan_.relocateTo(engine_, other_);
		}

		public: constexpr auto vacant(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->begin(engine_) == this->end(engine_);
		}

		public: constexpr auto lightBeforeBegin(Engine& engine_) const noexcept -> ConstLightIterator {
			return ConstLightIterator(this->m_sentryHolder_.handle(engine_.pool()));
		}

		public: constexpr auto lightBeforeBegin(Engine& engine_) noexcept -> LightIterator {
			return LightIterator(this->m_sentryHolder_.handle(engine_.pool()));
		}

		public: constexpr auto lightBegin(Engine& engine_) const noexcept -> ConstLightIterator {
			return this->lightBeforeBegin(engine_).next(engine_);
		}

		public: constexpr auto lightBegin(Engine& engine_) noexcept -> LightIterator {
			return this->lightBeforeBegin(engine_).next(engine_);
		}

		public: constexpr auto lightEnd(Engine& engine_) const noexcept -> ConstLightIterator {
			return this->lightBeforeBegin(engine_);
		}

		public: constexpr auto lightEnd(Engine& engine_) noexcept -> LightIterator {
			return this->lightBeforeBegin(engine_);
		}

		public: static constexpr auto fromLight(
			Engine& engine_, ConstLightIterator lightIterator_
		) noexcept -> ConstIterator {
			return ConstIterator(lightIterator_, engine_);
		}

		public: static constexpr auto fromLight(Engine& engine_, LightIterator lightIterator_) noexcept -> Iterator {
			return Iterator(lightIterator_, engine_);
		}

		public: static constexpr auto toLight(Engine& engine_, ConstIterator iterator_) noexcept -> ConstLightIterator {
			return static_cast<ConstLightIterator>(iterator_);
		}

		public: static constexpr auto toLight(Engine& engine_, Iterator iterator_) noexcept -> LightIterator {
			return static_cast<LightIterator>(iterator_);
		}

		public: constexpr auto beforeBegin(Engine& engine_) const noexcept -> ConstIterator {
			return fromLight(engine_, this->lightBeforeBegin(engine_));
		}

		public: constexpr auto beforeBegin(Engine& engine_) noexcept -> Iterator {
			return fromLight(engine_, this->lightBeforeBegin(engine_));
		}

		public: constexpr auto begin(Engine& engine_) const noexcept -> ConstIterator {
			return fromLight(engine_, this->lightBegin(engine_));
		}

		public: constexpr auto begin(Engine& engine_) noexcept -> Iterator {
			return fromLight(engine_, this->lightBegin(engine_));
		}

		public: constexpr auto end(Engine& engine_) const noexcept -> ConstIterator {
			return fromLight(engine_, this->lightEnd(engine_));
		}

		public: constexpr auto end(Engine& engine_) noexcept -> Iterator {
			return fromLight(engine_, this->lightEnd(engine_));
		}

		public: constexpr auto front(Engine& engine_) const& noexcept -> Value const& {
			return *(this->begin(engine_));
		}

		public: constexpr auto front(Engine& engine_)& noexcept -> Value& {
			return *(this->begin(engine_));
		}

		public: constexpr auto back(Engine& engine_) const& noexcept -> Value const& {
			return *(this->end(engine_).previous());
		}

		public: constexpr auto back(Engine& engine_)& noexcept -> Value& {
			return *(this->end(engine_).previous());
		}

		private: template <typename... ArgumentTs__> static constexpr auto createNode_(
			Pool& pool_, ArgumentTs__&&... parameters_
		) -> Handle {
			Handle result_ = PoolAdaptorForNode_::allocate(pool_);
			try {
				Node* pointer_ = static_cast<Node*>(PoolAdaptorForNode_::handleConverter(pool_)(result_));
				new (pointer_) Node(::dcool::core::inPlace, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			} catch (...) {
				PoolAdaptorForNode_::deallocate(pool_, result_);
				throw;
			}
			return result_;
		}

		private: template <typename... ArgumentTs__> static constexpr auto createNode_(
			Engine& engine_, ArgumentTs__&&... parameters_
		) -> Handle {
			return createNode_(engine_.pool(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		private: static constexpr void destroyNode_(Pool& pool_, Handle nodeHandle_) {
			auto node_ = static_cast<Node*>(PoolAdaptorForNode_::fromHandle(pool_, nodeHandle_));
			node_->~Node();
			PoolAdaptorForNode_::deallocate(pool_, nodeHandle_);
		}

		private: static constexpr void destroyNode_(Engine& engine_, Handle nodeHandle_) {
			destroyNode_(engine_.pool(), nodeHandle_);
		}

		private: constexpr auto insertNodeHeader_(
			HeaderHandleConverter const& headerConverter_, LightIterator position_, NodeHeader& headerToInsert_
		) noexcept -> LightIterator requires (bidirectional) {
			NodeHeader& tailing_ = position_.nodeHeader(headerConverter_);
			::dcool::container::detail_::insertLinkedNodeHeaderBefore_(tailing_, headerToInsert_, headerConverter_);
			return LightIterator(tailing_.previousHandle());
		}

		private: constexpr auto insertNode_(
			HeaderHandleConverter const& headerConverter_, LightIterator position_, Node& toInsert_
		) noexcept -> LightIterator requires (bidirectional) {
			return this->insertNodeHeader_(headerConverter_, position_, toInsert_.header());
		}

		public: constexpr auto insertNode(
			Engine& engine_, LightIterator position_, Node& toInsert_
		) noexcept -> LightIterator requires (bidirectional) {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			return this->insertNode_(headerConverter_, position_, toInsert_);
		}

		// Note that 'toInsert_' must point to a node instead of a node header
		public: constexpr auto insertNode(
			Engine& engine_, LightIterator position_, Handle toInsert_
		) noexcept -> LightIterator requires (bidirectional) {
			return this->insertNode(
				engine_, position_, *static_cast<Node*>(PoolAdaptorForNode_::fromHandle(engine_.pool(), toInsert_))
			);
		}

		public: constexpr auto insertNode(Iterator position_, Node& toInsert_) noexcept -> Iterator requires (bidirectional) {
			return this->insertNode_(position_.converter(), static_cast<LightIterator>(position_), toInsert_);
		}

		private: constexpr auto insertNodeHeaderAfter_(
			HeaderHandleConverter const& headerConverter_, LightIterator position_, NodeHeader& headerToInsert_
		) noexcept -> LightIterator {
			NodeHeader& leading_ = position_.nodeHeader(headerConverter_);
			::dcool::container::detail_::insertLinkedNodeHeaderAfter_(leading_, headerToInsert_, headerConverter_);
			return LightIterator(leading_.nextHandle());
		}

		private: constexpr auto insertNodeAfter_(
			HeaderHandleConverter const& headerConverter_, LightIterator position_, Node& toInsert_
		) noexcept -> LightIterator {
			return this->insertNodeHeaderAfter_(headerConverter_, position_, toInsert_.header());
		}

		public: constexpr auto insertNodeAfter(
			Engine& engine_, LightIterator position_, Node& toInsert_
		) noexcept -> LightIterator {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			return this->insertNodeAfter_(headerConverter_, position_, toInsert_);
		}

		// Note that 'toInsert_' must point to a node instead of a node header
		public: constexpr auto insertNodeAfter(
			Engine& engine_, LightIterator position_, Handle toInsert_
		) noexcept -> LightIterator {
			return this->insertNodeAfter(
				engine_, position_, *static_cast<Node*>(PoolAdaptorForNode_::fromHandle(engine_.pool(), toInsert_))
			);
		}

		public: constexpr auto insertNodeAfter(Iterator position_, Node& toInsert_) noexcept -> Iterator {
			return this->insertNodeAfter_(position_.converter(), static_cast<LightIterator>(position_), toInsert_);
		}

		public: constexpr auto popNode(Engine& engine_, LightIterator position_) noexcept -> Handle requires (bidirectional) {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			return ::dcool::resource::adaptedToHandleFor<Node>(
				engine_.pool(),
				static_cast<Node*>(
					::dcool::core::addressOf(
						::dcool::container::detail_::popLinkedNodeHeader_(
							position_.nodeHeader(headerConverter_), headerConverter_
						)
					)
				)
			);
		}

		public: constexpr auto popNodeAfter(Engine& engine_, LightIterator positionBefore_) noexcept -> Handle {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool());
			return ::dcool::resource::adaptedToHandleFor<Node>(
				engine_.pool(),
				static_cast<Node*>(
					::dcool::core::addressOf(
						::dcool::container::detail_::popLinkedNodeHeaderAfter_(
							positionBefore_.nodeHeader(headerConverter_), headerConverter_
						)
					)
				)
			);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Engine& engine_, LightIterator position_, ArgumentTs__&&... parameters_
		) -> LightIterator requires (bidirectional) {
			Handle nodeHandle_ = createNode_(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			return this->insertNode(engine_, position_, nodeHandle_);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator requires (bidirectional) {
			LightIterator result_ = this->emplace(
				engine_, this->toLight(engine_, position_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
			return fromLight(engine_, result_);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceAfter(
			Engine& engine_, LightIterator position_, ArgumentTs__&&... parameters_
		) -> LightIterator {
			Handle nodeHandle_ = createNode_(engine_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
			return this->insertNodeAfter(engine_, position_, nodeHandle_);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceAfter(
			Engine& engine_, Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			LightIterator result_ = this->emplaceAfter(
				engine_, this->toLight(engine_, position_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
			return fromLight(engine_, result_);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceFront(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplaceAfter(engine_, this->beforeBegin(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceBack(
			Engine& engine_, ArgumentTs__&&... parameters_
		) requires (bidirectional) {
			this->emplace(engine_, this->end(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr auto erase_(Pool& pool_, LightIterator position_) noexcept -> LightIterator requires (bidirectional) {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(pool_);
			NodeHeader& erasedNodeHeader_ = ::dcool::container::detail_::popLinkedNodeHeader_(
				position_.nodeHeader(headerConverter_), headerConverter_
			);
			destroyNode_(
				pool_, PoolAdaptorForNode_::toHandle(pool_, ::dcool::core::addressOf(Node::retrieveFromHeader(erasedNodeHeader_)))
			);
			return position_.next(headerConverter_);
		}

		public: constexpr auto erase(Engine& engine_, LightIterator position_) noexcept -> LightIterator requires (bidirectional) {
			return this->erase_(engine_.pool(), position_);
		}

		public: constexpr auto erase(Engine& engine_, Iterator position_) noexcept -> Iterator requires (bidirectional) {
			LightIterator result_ = this->erase_(engine_.pool(), this->toLight(engine_, position_));
			return this->fromLight(engine_, result_);
		}

		public: constexpr auto eraseAfter_(Pool& pool_, LightIterator positionBefore_) noexcept -> LightIterator {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(pool_);
			NodeHeader& erasedNodeHeader_ = ::dcool::container::detail_::popLinkedNodeHeaderAfter_(
				positionBefore_.nodeHeader(headerConverter_), headerConverter_
			);
			destroyNode_(
				pool_, PoolAdaptorForNode_::toHandle(pool_, ::dcool::core::addressOf(Node::retrieveFromHeader(erasedNodeHeader_)))
			);
			return positionBefore_.next(headerConverter_);
		}

		public: constexpr auto eraseAfter(Engine& engine_, LightIterator position_) noexcept -> LightIterator {
			return this->eraseAfter_(engine_.pool(), position_);
		}

		public: constexpr auto eraseAfter(Engine& engine_, Iterator position_) noexcept -> Iterator {
			LightIterator result_ = this->eraseAfter_(engine_.pool(), this->toLight(engine_, position_));
			return this->fromLight(engine_, result_);
		}

		public: constexpr void popFront(Engine& engine_) noexcept {
			this->eraseAfter(engine_, this->beforeBegin(engine_));
		}

		public: constexpr void popBack(Engine& engine_) noexcept requires (bidirectional) {
			this->erase(engine_, this->end(engine_).previous());
		}
	};

	template <
		::dcool::core::Object ValueT_, ::dcool::container::LinkedConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct Linked {
		private: using Self_ = Linked<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::container::LinkedConfigAdaptor<Config, Value>;
		public: using Chassis = ::dcool::container::LinkedChassis<Value, Config>;
		public: using Node = ConfigAdaptor_::LinkedNode;
		public: using NodeHeader = ConfigAdaptor_::LinkedNodeHeader;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Iterator = ConfigAdaptor_::LinkedIterator;
		public: using ConstIterator = ConfigAdaptor_::LinkedConstIterator;
		public: using LightIterator = ConfigAdaptor_::LinkedLightIterator;
		public: using ConstLightIterator = ConfigAdaptor_::LinkedLightConstIterator;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean bidirectional = Chassis::bidirectional;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		public: constexpr Linked() noexcept {
			this->chassis().initialize(this->engine());
		}

		public: constexpr Linked(Self_ const& other_): m_engine_(other_.m_engine_) {
			other_.chassis().cloneTo(other_.engine(), this->engine(), this->chassis());
		}

		public: constexpr Linked(Self_&& other_) noexcept: m_engine_(other_.m_engine_) {
			other_.chassis().relocateTo(other_.engine(), this->chassis());
			try {
				other_.initialize(other_.engine());
			} catch (...) {
				this->chassis().relocateTo(this->engine(), other_.chassis());
				throw;
			}
		}

		public: constexpr ~Linked() noexcept {
			this->chassis().uninitialize(this->engine());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(this->engine(), other_.engine(), other_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) -> Self_& {
			this->engine() = other_.engine();
			other_.chassis().relocateTo(other_.engine(), this->chassis());
			return *this;
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			::dcool::core::intelliSwap(this->engine(), other_.engine());
			this->chassis().swapWith(other_.engine(), this->engine(), other_.chassis());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto lightBeforeBegin() const noexcept -> ConstLightIterator {
			return this->chassis().lightBeforeBegin(this->engine());
		}

		public: constexpr auto lightBeforeBegin() noexcept -> LightIterator {
			return this->chassis().lightBeforeBegin(this->engine());
		}

		public: constexpr auto lightBegin() const noexcept -> ConstLightIterator {
			return this->chassis().lightBegin(this->engine());
		}

		public: constexpr auto lightBegin() noexcept -> LightIterator {
			return this->chassis().lightBegin(this->engine());
		}

		public: constexpr auto lightEnd() const noexcept -> ConstLightIterator {
			return this->chassis().lightEnd(this->engine());
		}

		public: constexpr auto lightEnd() noexcept -> LightIterator {
			return this->chassis().lightEnd(this->engine());
		}

		public: constexpr auto beforeBegin() const noexcept -> ConstIterator {
			return this->chassis().beforeBegin(this->engine());
		}

		public: constexpr auto beforeBegin() noexcept -> Iterator {
			return this->chassis().beforeBegin(this->engine());
		}

		public: constexpr auto begin() const noexcept -> ConstIterator {
			return this->chassis().begin(this->engine());
		}

		public: constexpr auto begin() noexcept -> Iterator {
			return this->chassis().begin(this->engine());
		}

		public: constexpr auto end() const noexcept -> ConstIterator {
			return this->chassis().end(this->engine());
		}

		public: constexpr auto end() noexcept -> Iterator {
			return this->chassis().end(this->engine());
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			LightIterator position_, ArgumentTs__&&... parameters_
		) -> LightIterator requires (bidirectional) {
			return this->chassis().emplace(this->engine(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplace(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator requires (bidirectional) {
			return this->chassis().emplace(this->engine(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceAfter(
			LightIterator position_, ArgumentTs__&&... parameters_
		) -> LightIterator {
			return this->chassis().emplaceAfter(this->engine(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr auto emplaceAfter(
			Iterator position_, ArgumentTs__&&... parameters_
		) -> Iterator {
			return this->chassis().emplaceAfter(this->engine(), position_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceFront(ArgumentTs__&&... parameters_) {
			this->chassis().emplaceFront(this->engine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceBack(ArgumentTs__&&... parameters_) {
			this->chassis().emplaceBack(this->engine(), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr auto erase(LightIterator position_) noexcept -> LightIterator requires (bidirectional) {
			return this->chassis().erase(this->engine(), position_);
		}

		public: constexpr auto erase(Iterator position_) noexcept -> Iterator requires (bidirectional) {
			return this->chassis().erase(this->engine(), position_);
		}

		public: constexpr auto eraseAfter(LightIterator position_) noexcept -> LightIterator {
			return this->chassis().eraseAfter(this->engine(), position_);
		}

		public: constexpr auto eraseAfter(Iterator position_) noexcept -> Iterator {
			return this->chassis().eraseAfter(this->engine(), position_);
		}
	};

	template <
		typename ValueT_, ::dcool::container::BidirectionalLinkedConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct BidirectionalLinkedChassis: public ::dcool::container::LinkedChassis<
		ValueT_, ::dcool::container::detail_::BidirectionalLinkedUnderlyingConfig_<ConfigT_, ValueT_>
	> {
		public: using Config = ConfigT_;

		private: using Super_ = ::dcool::container::LinkedChassis<
			ValueT_, ::dcool::container::detail_::BidirectionalLinkedUnderlyingConfig_<ConfigT_, ValueT_>
		>;

		public: using Super_::Super_;
	};

	template <
		typename ValueT_, ::dcool::container::BidirectionalLinkedConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct BidirectionalLinked: public ::dcool::container::Linked<
		ValueT_, ::dcool::container::detail_::BidirectionalLinkedUnderlyingConfig_<ConfigT_, ValueT_>
	> {
		public: using Config = ConfigT_;

		private: using Super_ = ::dcool::container::Linked<
			ValueT_, ::dcool::container::detail_::BidirectionalLinkedUnderlyingConfig_<ConfigT_, ValueT_>
		>;

		public: using Super_::Super_;
	};
}

#endif
