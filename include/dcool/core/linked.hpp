#ifndef DCOOL_CORE_LINKED_HPP_INCLUDED_
#	define DCOOL_CORE_LINKED_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/utility.hpp>
#	include <dcool/core/memory.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/node.hpp>
#	include <dcool/core/type_value_detector.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::core::detail_, HasTypeDefaultForwardLinkedNode_, ExtractedForwardLinkedNodeType_, ForwardLinkedNode
)

namespace dcool::core {
	namespace detail_ {
		template <typename NodeHeaderT_> using LinkedNodeHeaderHandleType_ =
			::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<NodeHeaderT_&>().nextHandle())>
		;

		template <typename NodeT_> using LinkedNodeHandleType_ = ::dcool::core::detail_::LinkedNodeHeaderHandleType_<
			::dcool::core::NodeHeaderType<NodeT_>
		>;
	}

	template <
		typename T_,
		typename HandleConverterT_ = ::dcool::core::DefaultBidirectionalImplicitConverter<
			::dcool::core::detail_::LinkedNodeHeaderHandleType_<T_>, void*
		>
	> concept ForwardLinkedNodeHeader = ::dcool::core::BidirectionalConsistentConverter<
		HandleConverterT_, ::dcool::core::detail_::LinkedNodeHeaderHandleType_<T_>, void*
	> && requires (T_ nodeHeader_, ::dcool::core::detail_::LinkedNodeHeaderHandleType_<T_> handle_) {
		handle_ = nodeHeader_.nextHandle();
		nodeHeader_.setNextHandle(handle_);
	};

	template <typename T_, typename HandleConverterT_> concept ForwardLinkedNode =
		::dcool::core::ValueNode<T_> && ::dcool::core::ForwardLinkedNodeHeader<::dcool::core::NodeHeaderType<T_>, HandleConverterT_>
	;

	namespace detail_ {
		template <typename NodeHeaderT_, typename ConverterT_> constexpr auto nextForwardNodeHeader_(
			NodeHeaderT_& leading_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			return *static_cast<NodeHeaderT_*>(converter_(leading_.nextHandle()));
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr void connectForwardNodeHeader_(
			NodeHeaderT_& leading_, NodeHeaderT_& following_, ConverterT_ const& converter_
		) noexcept {
			leading_.setNextHandle(converter_(static_cast<void*>(::dcool::core::addressOf(following_))));
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr void insertForwardNodeHeaderAfter_(
			NodeHeaderT_& leading_, NodeHeaderT_& toInsert_, ConverterT_ const& converter_
		) noexcept {
			toInsert_.setNextHandle(leading_.nextHandle());
			::dcool::core::detail_::connectForwardNodeHeader_(leading_, toInsert_, converter_);
		}

		template <typename NodeHeaderT_, typename ConverterT_> constexpr auto popForwardNodeHeaderAfter_(
			NodeHeaderT_& leading_, ConverterT_ const& converter_
		) noexcept -> NodeHeaderT_& {
			NodeHeaderT_& result_ = ::dcool::core::detail_::nextForwardNodeHeader_(leading_, converter_);
			leading_.setNextHandle(result_.nextHandle());
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

		template <
			typename HandleT_, typename ValueT_, typename DistinguisherT_ = void
		> struct DefaultForwardLinkedNode_: public ::dcool::core::detail_::DefaultForwardLinkedNodeHeader_<HandleT_> {
			private: using Self_ = DefaultForwardLinkedNode_<HandleT_, ValueT_, DistinguisherT_>;
			private: using Super_ = ::dcool::core::detail_::DefaultForwardLinkedNodeHeader_<HandleT_>;
			public: using Value = ValueT_;

			public: using Header = Super_;

			private: Value m_value_;

			public: template <typename... ArgumentTs__> constexpr DefaultForwardLinkedNode_(
				ArgumentTs__&&... parameters_
			) noexcept(noexcept(Value(::dcool::core::forward<ArgumentTs__>(parameters_)...))): m_value_(
				::dcool::core::forward<ArgumentTs__>(parameters_)...
			) {
			}

			public: constexpr auto value() const noexcept -> Value const& {
				return this->m_value_;
			}

			public: constexpr auto value() noexcept -> Value& {
				return this->m_value_;
			}

			public: constexpr auto header() const noexcept -> Header const& {
				return *this;
			}

			public: constexpr auto header() noexcept -> Header& {
				return *this;
			}

			public: static constexpr auto retrieveFromHeader(Header const& header_) noexcept -> Self_ const& {
				return static_cast<Self_ const&>(header_);
			}

			public: static constexpr auto retrieveFromHeader(Header& header_) noexcept -> Self_& {
				return static_cast<Self_&>(header_);
			}
		};

		template <typename ValueT_> struct HandleConverterOfEngineForHelper_ {
			template <typename EngineT__> static constexpr auto get_(EngineT__& engine_) noexcept {
				return ::dcool::core::adaptedHandleConverterFor<ValueT_>(engine_.pool);
			}
		};

		template <typename ValueT_> struct HandleConverterOfEngineForHelper_<ValueT_ const> {
			template <typename EngineT__> static constexpr auto get_(EngineT__& engine_) noexcept {
				return ::dcool::core::adaptedConstHandleConverterFor<ValueT_>(engine_.pool);
			}
		};

		template <typename EngineT_, typename ValueT_> struct UnifiedHandleOf_ {
			using Result_ = ::dcool::core::PoolAdaptorFor<
				decltype(::dcool::core::declval<EngineT_&>().pool), ValueT_
			>::UnifiedHandle;
		};

		template <typename EngineT_, typename ValueT_> struct UnifiedHandleOf_<EngineT_, ValueT_ const> {
			using Result_ = ::dcool::core::PoolAdaptorFor<
				decltype(::dcool::core::declval<EngineT_&>().pool), ValueT_
			>::UnifiedConstHandle;
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeHeaderT_
		> struct ForwardLinkedHeaderIteratorBase_ {
			private: using Self_ = ForwardLinkedHeaderIteratorBase_<HandleConverterT_, EngineT_, NodeHeaderT_>;
			public: using HandleConverter = HandleConverterT_;
			public: using Engine = EngineT_;
			public: using NodeHeader = NodeHeaderT_;

			public: using Handle = ::dcool::core::detail_::UnifiedHandleOf_<EngineT_, NodeHeaderT_>::Result_;

			private: Handle m_handle_;

			public: ForwardLinkedHeaderIteratorBase_() = delete;
			public: constexpr ForwardLinkedHeaderIteratorBase_(Self_ const&) noexcept = default;
			public: constexpr ForwardLinkedHeaderIteratorBase_(Self_&&) noexcept = default;
			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: constexpr explicit ForwardLinkedHeaderIteratorBase_(Handle handle_) noexcept: m_handle_(handle_) {
			}

			public: constexpr auto nodeHeaderHandle() const noexcept -> Handle {
				return this->m_handle_;
			}

			public: constexpr auto nodeHeader(HandleConverter const& converter_) const noexcept -> NodeHeader& {
				return *static_cast<NodeHeader*>(converter_(this->nodeHeaderHandle()));
			}

			public: constexpr auto nodeHeader(Engine& engine_) const noexcept -> NodeHeader& {
				HandleConverter converter_ = ::dcool::core::detail_::HandleConverterOfEngineForHelper_<NodeHeader>::get_(engine_);
				return this->nodeHeader(converter_);
			}

			public: constexpr auto nextHandle(HandleConverter const& converter_) const noexcept -> Handle {
				return this->nodeHeader(converter_).nextHandle();
			}

			public: constexpr auto nextHandle(Engine& engine_) const noexcept -> Handle {
				return this->nodeHeader(engine_).nextHandle();
			}

			public: constexpr void advance(HandleConverter const& converter_) noexcept {
				this->m_handle_ = this->nextHandle(converter_);
			}

			public: constexpr void advance(Engine& engine_) noexcept {
				this->m_handle_ = this->nextHandle(engine_);
			}

			public: constexpr auto next(HandleConverter const& converter_) const noexcept -> Self_ {
				return Self_(this->nextHandle(converter_));
			}

			public: constexpr auto next(Engine& engine_) const noexcept -> Self_ {
				return Self_(this->nextHandle(engine_));
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
			typename HandleConverterT_, typename EngineT_, typename NodeT_
		> struct ForwardLinkedIterator_;

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeT_
		> struct ForwardLinkedLightIterator_: private ::dcool::core::detail_::ForwardLinkedHeaderIteratorBase_<
			HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>
		> {
			private: using Self_ = ForwardLinkedLightIterator_<HandleConverterT_, EngineT_, NodeT_>;
			private: using Super_ = ::dcool::core::detail_::ForwardLinkedHeaderIteratorBase_<
				HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>
			>;
			public: using Node = NodeT_;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;

			public: using Value = ::dcool::core::NodeValueType<Node>;
			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::Handle;
			public: using Super_::nodeHeaderHandle;
			public: using Super_::nodeHeader;
			public: using Super_::nextHandle;
			public: using Super_::advance;
			private: using Super_::next;
			public: using Independent = ::dcool::core::detail_::ForwardLinkedIterator_<HandleConverter, Engine, Node>;

			public: ForwardLinkedLightIterator_() = delete;
			public: constexpr ForwardLinkedLightIterator_(Self_ const&) noexcept = default;
			public: constexpr ForwardLinkedLightIterator_(Self_&&) noexcept = default;
			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: constexpr explicit ForwardLinkedLightIterator_(Handle handle_) noexcept: Super_(handle_) {
			}

			private: template <
				::dcool::core::FormOfSame<Super_> SuperT__
			> constexpr explicit ForwardLinkedLightIterator_(SuperT__&& super_) noexcept: Super_(
				::dcool::core::forward<SuperT__>(super_)
			) {
			}

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
			typename HandleConverterT_, typename EngineT_, typename NodeHeaderT_
		> struct ForwardLinkedHeaderIterator_: private ::dcool::core::detail_::ForwardLinkedHeaderIteratorBase_<
			HandleConverterT_, EngineT_, NodeHeaderT_
		> {
			private: using Self_ = ForwardLinkedHeaderIterator_<HandleConverterT_, EngineT_, NodeHeaderT_>;
			private: using Super_ = ::dcool::core::detail_::ForwardLinkedHeaderIteratorBase_<
				HandleConverterT_, EngineT_, NodeHeaderT_
			>;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;
			private: using NodeHeader_ = NodeHeaderT_;

			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::NodeHeader;
			public: using typename Super_::Handle;

			private: [[no_unique_address]] HandleConverter m_converter_;

			public: ForwardLinkedHeaderIterator_() = delete;
			public: constexpr ForwardLinkedHeaderIterator_(Self_ const&) noexcept = default;
			public: constexpr ForwardLinkedHeaderIterator_(Self_&&) noexcept = default;
			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: template <::dcool::core::FormOfSame<HandleConverter> ConverterT__> constexpr ForwardLinkedHeaderIterator_(
				Handle handle_, ConverterT__&& converter_
			) noexcept: Super_(handle_), m_converter_(::dcool::core::forward<ConverterT__>(converter_)) {
			}

			private: template <
				::dcool::core::FormOfSame<Super_> SuperT__, ::dcool::core::FormOfSame<HandleConverter> ConverterT__
			> constexpr ForwardLinkedHeaderIterator_(
				SuperT__&& super_, ConverterT__&& converter_
			) noexcept:
				Super_(::dcool::core::forward<SuperT__>(super_)), m_converter_(::dcool::core::forward<ConverterT__>(converter_))
			{
			}

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

			public: constexpr void advance() noexcept {
				this->Super_::advance(this->converter());
			}

			public: constexpr auto next() const noexcept -> Self_ {
				return Self_(this->Super_::next(this->converter()), this->converter());
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

			public: friend auto operator !=(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean = default;
		};

		template <
			typename HandleConverterT_, typename EngineT_, typename NodeT_
		> struct ForwardLinkedIterator_: private ::dcool::core::detail_::ForwardLinkedHeaderIterator_<
			HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>
		> {
			private: using Self_ = ForwardLinkedIterator_<HandleConverterT_, EngineT_, NodeT_>;
			private: using Super_ = ::dcool::core::detail_::ForwardLinkedHeaderIterator_<
				HandleConverterT_, EngineT_, ::dcool::core::NodeHeaderType<NodeT_>
			>;
			private: using HandleConverter_ = HandleConverterT_;
			private: using Engine_ = EngineT_;
			public: using Node = NodeT_;

			public: using Value = ::dcool::core::NodeValueType<Node>;
			public: using typename Super_::NodeHeader;
			public: using typename Super_::HandleConverter;
			public: using typename Super_::Engine;
			public: using typename Super_::Handle;
			public: using Super_::converter;
			public: using Super_::nodeHeaderHandle;
			public: using Super_::nodeHeader;
			public: using Super_::nextHandle;
			public: using Super_::advance;
			private: using Super_::next;
			public: using Light = ::dcool::core::detail_::ForwardLinkedLightIterator_<HandleConverter, EngineT_, Node>;

			public: ForwardLinkedIterator_() = delete;
			public: constexpr ForwardLinkedIterator_(Self_ const&) noexcept = default;
			public: constexpr ForwardLinkedIterator_(Self_&&) noexcept = default;
			public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = default;
			public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

			public: template <::dcool::core::FormOfSame<HandleConverter> ConverterT__> constexpr ForwardLinkedIterator_(
				Handle handle_, ConverterT__&& converter_
			) noexcept: Super_(handle_, ::dcool::core::forward<ConverterT__>(converter_)) {
			}

			public: constexpr ForwardLinkedIterator_(Light const& light_, Engine& engine_) noexcept: Self_(
				light_.nodeHeaderHandle(), ::dcool::core::detail_::HandleConverterOfEngineForHelper_<NodeHeader>::get_(engine_)
			) {
			}

			private: template <::dcool::core::FormOfSame<Super_> SuperT__> constexpr explicit ForwardLinkedIterator_(
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

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->advance();
				return *this;
			}

			public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
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

			public: friend auto operator !=(Self_ const& first_, Self_ const& second_) noexcept -> ::dcool::core::Boolean = default;
		};

		template <
			typename ValueT_, typename ConfigT_
		> using ForwardLinkedNodeType_ = ::dcool::core::detail_::ExtractedForwardLinkedNodeType_<
			ConfigT_,
			::dcool::core::detail_::DefaultForwardLinkedNode_<
				typename ::dcool::core::PoolType<ConfigT_>::UnifiedHandle, ValueT_, ::dcool::core::Empty<ValueT_, ConfigT_>
			>
		>;

		template <typename NodeHeaderT_, typename PoolT_, typename EngineT_> struct LinkedSentryHolder_ {
			private: using Self_ = LinkedSentryHolder_<NodeHeaderT_, PoolT_, EngineT_>;
			public: using Pool = PoolT_;
			public: using NodeHeader = NodeHeaderT_;
			public: using Engine = EngineT_;

			private: using PoolAdaptor_ = ::dcool::core::PoolAdaptorFor<Pool, NodeHeader>;
			public: using Handle = PoolAdaptor_::UnifiedHandle;
			public: using ConstHandle = PoolAdaptor_::UnifiedConstHandle;
			public: using HandleConverter = PoolAdaptor_::HandleConverter;
			public: using ConstHandleConverter = PoolAdaptor_::ConstHandleConverter;
			public: static constexpr ::dcool::core::Boolean noexceptInitializeable = false;

			private: Handle m_sentry_;

			public: constexpr LinkedSentryHolder_() noexcept = default;
			public: LinkedSentryHolder_(Self_ const&) = delete;
			public: LinkedSentryHolder_(Self_&&) = delete;
			public: constexpr ~LinkedSentryHolder_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: constexpr void initialize(Engine& engine_) {
				this->m_sentry_ = PoolAdaptor_::allocate(engine_.pool);
				new (this->sentryAddress_(engine_)) NodeHeader();
			}

			public: constexpr void relocateTo(Self_& other_) {
				other_.m_sentry_ = this->m_sentry_;
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
				PoolAdaptor_::deallocate(engine_.pool, this->m_sentry_);
			}

			public: constexpr auto sentryHandle(Engine& engine_) const noexcept -> ConstHandle {
				return this->m_sentry_;
			}

			public: constexpr auto sentryHandle(Engine& engine_) noexcept -> Handle {
				return this->m_sentry_;
			}

			private: constexpr auto sentryAddress_(Engine& engine_) const noexcept -> NodeHeader const* {
				return static_cast<NodeHeader const*>(PoolAdaptor_::constHandleConverter(engine_.pool)(this->m_sentry_));
			}

			private: constexpr auto sentryAddress_(Engine& engine_) noexcept -> NodeHeader* {
				return static_cast<NodeHeader*>(PoolAdaptor_::handleConverter(engine_.pool)(this->m_sentry_));
			}

			public: constexpr auto sentry(Engine& engine_) const noexcept -> NodeHeader const& {
				return *(this->sentryAddress_(engine_));
			}

			public: constexpr auto sentry(Engine& engine_) noexcept -> NodeHeader& {
				return *(this->sentryAddress_(engine_));
			}
		};

		template <
			typename NodeHeaderT_, ::dcool::core::PoolWithBijectiveHandleConverterFor<NodeHeaderT_> PoolT_, typename EngineT_
		> struct LinkedSentryHolder_<NodeHeaderT_, PoolT_, EngineT_> {
			private: using Self_ = LinkedSentryHolder_<NodeHeaderT_, PoolT_, EngineT_>;
			public: using Pool = PoolT_;
			public: using NodeHeader = NodeHeaderT_;
			public: using Engine = EngineT_;

			private: using PoolAdaptor_ = ::dcool::core::PoolAdaptorFor<Pool, NodeHeader>;
			public: using Handle = PoolAdaptor_::UnifiedHandle;
			public: using ConstHandle = PoolAdaptor_::UnifiedConstHandle;
			public: using HandleConverter = PoolAdaptor_::HandleConverter;
			public: using ConstHandleConverter = PoolAdaptor_::ConstHandleConverter;
			public: static constexpr ::dcool::core::Boolean noexceptInitializeable = true;

			private: NodeHeader m_sentry_;

			public: constexpr LinkedSentryHolder_() noexcept = default;
			public: LinkedSentryHolder_(Self_ const&) = delete;
			public: LinkedSentryHolder_(Self_&&) = delete;
			public: constexpr ~LinkedSentryHolder_() noexcept = default;
			public: auto operator =(Self_ const&) -> Self_& = delete;
			public: auto operator =(Self_&&) -> Self_& = delete;

			public: constexpr void initialize(Engine& engine_) noexcept {
			}

			public: constexpr void uninitialize(Engine& engine_) noexcept {
			}

			public: constexpr void relocateTo(Self_& other_) noexcept {
				other_.m_sentry_ = this->m_sentry_;
			}

			public: constexpr auto sentryHandle(Engine& engine_) const noexcept -> ConstHandle {
				return PoolAdaptor_::constHandleConverter(engine_.pool)(static_cast<void const*>(&(this->m_sentry_)));
			}

			public: constexpr auto sentryHandle(Engine& engine_) noexcept -> Handle {
				return PoolAdaptor_::handleConverter(engine_.pool)(static_cast<void*>(&(this->m_sentry_)));
			}

			public: constexpr auto sentry(Engine& engine_) const noexcept -> NodeHeader const& {
				return this->m_sentry_;
			}

			public: constexpr auto sentry(Engine& engine_) noexcept -> NodeHeader& {
				return this->m_sentry_;
			}
		};

		template <typename ValueT_, typename ConfigT_> struct ForwardLinkedConfigAdaptorHelper_ {
			using Config = ConfigT_;
			using Value = ValueT_;

			using Pool = ::dcool::core::PoolType<Config>;

			struct Engine {
				public: [[no_unique_address]] Pool pool;
			};

			using ForwardLinkedNode = ::dcool::core::detail_::ForwardLinkedNodeType_<Value, Config>;
			using ForwardLinkedNodeHeader = ::dcool::core::NodeHeaderType<ForwardLinkedNode>;
			using PoolAdaptorForForwardLinkedNode = ::dcool::core::PoolAdaptorFor<Pool, ForwardLinkedNode>;
			using PoolAdaptorForForwardLinkedNodeHeader = ::dcool::core::PoolAdaptorFor<Pool, ForwardLinkedNodeHeader>;
			using Handle = PoolAdaptorForForwardLinkedNode::UnifiedHandle;
			using ConstHandle = PoolAdaptorForForwardLinkedNode::UnifiedConstHandle;
			using HandleConverter = PoolAdaptorForForwardLinkedNode::HandleConverter;
			using ConstHandleConverter = PoolAdaptorForForwardLinkedNode::ConstHandleConverter;
			using HeaderHandleConverter = PoolAdaptorForForwardLinkedNodeHeader::HandleConverter;
			using HeaderConstHandleConverter = PoolAdaptorForForwardLinkedNodeHeader::ConstHandleConverter;
			using ForwardLinkedIterator = ::dcool::core::detail_::ForwardLinkedIterator_<
				HeaderHandleConverter, Engine, ForwardLinkedNode
			>;
			using ForwardLinkedConstIterator = ::dcool::core::detail_::ForwardLinkedIterator_<
				HeaderConstHandleConverter, Engine, ForwardLinkedNode const
			>;
			using ForwardLinkedLightIterator = ForwardLinkedIterator::Light;
			using ForwardLinkedLightConstIterator = ForwardLinkedConstIterator::Light;
			using SentryHolder = ::dcool::core::detail_::LinkedSentryHolder_<ForwardLinkedNodeHeader, Pool, Engine>;
		};

		template <
			typename T_, typename ValueT_, typename AdaptorHelperT_ = ForwardLinkedConfigAdaptorHelper_<ValueT_, T_>
		> concept ForwardLinkedConfigWithAdaptorHelper_ =
			::dcool::core::Object<ValueT_> &&
			::dcool::core::PoolFor<typename AdaptorHelperT_::Pool, typename AdaptorHelperT_::ForwardLinkedNode> &&
			::dcool::core::PoolFor<typename AdaptorHelperT_::Pool, typename AdaptorHelperT_::ForwardLinkedNodeHeader> &&
			::dcool::core::SameAs<
				typename AdaptorHelperT_::Value, ::dcool::core::NodeValueType<typename AdaptorHelperT_::ForwardLinkedNode>
			> &&
			::dcool::core::SameAs<
				typename AdaptorHelperT_::Handle,
				::dcool::core::detail_::LinkedNodeHandleType_<typename AdaptorHelperT_::ForwardLinkedNode>
			>
		;
	}

	template <typename T_, typename ValueT_> concept ForwardLinkedConfigFor =
		::dcool::core::detail_::ForwardLinkedConfigWithAdaptorHelper_<T_, ValueT_>
	;

	template <
		::dcool::core::Object ValueT_, ::dcool::core::ForwardLinkedConfigFor<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct ForwardLinkedConfigAdaptor: ::dcool::core::detail_::ForwardLinkedConfigAdaptorHelper_<ValueT_, ConfigT_> {
	};

	template <
		::dcool::core::Object ValueT_, ::dcool::core::ForwardLinkedConfigFor<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct ForwardLinkedChassis {
		private: using Self_ = ForwardLinkedChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::core::ForwardLinkedConfigAdaptor<Value, Config>;
		public: using Node = ConfigAdaptor_::ForwardLinkedNode;
		public: using NodeHeader = ConfigAdaptor_::ForwardLinkedNodeHeader;
		public: using Pool = ConfigAdaptor_::Pool;
		private: using PoolAdaptorForNode_ = ConfigAdaptor_::PoolAdaptorForForwardLinkedNode;
		private: using PoolAdaptorForNodeHeader_ = ConfigAdaptor_::PoolAdaptorForForwardLinkedNodeHeader;
		public: using Handle = ConfigAdaptor_::Handle;
		public: using HandleConverter = ConfigAdaptor_::HandleConverter;
		public: using HeaderHandleConverter = ConfigAdaptor_::HeaderHandleConverter;
		public: using Iterator = ConfigAdaptor_::ForwardLinkedIterator;
		public: using ConstIterator = ConfigAdaptor_::ForwardLinkedConstIterator;
		public: using LightIterator = ConfigAdaptor_::ForwardLinkedLightIterator;
		public: using LightConstIterator = ConfigAdaptor_::ForwardLinkedLightConstIterator;
		public: using SentryHolder = ConfigAdaptor_::SentryHolder;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean noexceptInitializeable = SentryHolder::noexceptInitializeable;

		public: constexpr ForwardLinkedChassis() noexcept = default;
		public: ForwardLinkedChassis(Self_ const&) = delete;
		public: ForwardLinkedChassis(Self_&&) = delete;
		public: constexpr ~ForwardLinkedChassis() noexcept = default;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: SentryHolder m_sentryHolder_;

		public: constexpr void initialize(Engine& engine_) noexcept(noexceptInitializeable) {
			this->m_sentryHolder_.initialize(engine_);
			NodeHeader& sentry_ = this->m_sentryHolder_.sentry(engine_);
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool);
			::dcool::core::detail_::connectForwardNodeHeader_(sentry_, sentry_, headerConverter_);
		}

		public: constexpr void relocateTo(Self_& other_) noexcept {
			this->m_sentryHolder_.relocateTo(other_.m_sentryHolde_);
		}

		public: template <typename ValueT__, typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::core::ForwardLinkedChassis<ValueT__, ConfigT__>::Engine& otherEngine_,
			::dcool::core::ForwardLinkedChassis<ValueT__, ConfigT__>& other_
		) const {
			other_.initialize(engine_);
			try {
				LightConstIterator begin_ = this->lightBegin(engine_);
				LightConstIterator end_ = this->lightEnd(engine_);
				typename ::dcool::core::ForwardLinkedChassis<
					ValueT__, ConfigT__
				>::LightIterator current_ = other_.lightBegin(otherEngine_);
				for (; !(LightConstIterator::equal(begin_, end_, engine_)); begin_.advance(engine_)) {
					current_ = other_.emplaceAfter(otherEngine_, current_, begin_.dereferenceSelf(engine_));
				}
			} catch (...) {
				other_.uninitialize(engine_);
				throw;
			}
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool);
			HandleConverter converter_ = PoolAdaptorForNode_::handleConverter(engine_.pool);
			LightIterator begin_ = this->lightBegin(engine_);
			LightIterator end_ = this->lightEnd(engine_);
			while (!(begin_.equalsTo(end_, headerConverter_))) {
				Node* node_ = ::dcool::core::addressOf(begin_.node(headerConverter_));
				Handle nodeHandle_ = converter_(static_cast<void*>(node_));
				begin_.advance(headerConverter_);
				destroyNode_(engine_, nodeHandle_);
			}
			this->m_sentryHolder_.uninitialize(engine_);
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			Self_ middleMan_;
			this->relocateTo(middleMan_);
			other_.relocateTo(*this);
			middleMan_.relocateTo(other_);
		}

		public: constexpr auto lightBeforeBegin(Engine& engine_) const noexcept -> LightConstIterator {
			return LightConstIterator(this->m_sentryHolder_.sentryHandle(engine_));
		}

		public: constexpr auto lightBeforeBegin(Engine& engine_) noexcept -> LightIterator {
			return LightIterator(this->m_sentryHolder_.sentryHandle(engine_));
		}

		public: constexpr auto lightBegin(Engine& engine_) const noexcept -> LightConstIterator {
			return this->lightBeforeBegin(engine_).next(engine_);
		}

		public: constexpr auto lightBegin(Engine& engine_) noexcept -> LightIterator {
			return this->lightBeforeBegin(engine_).next(engine_);
		}

		public: constexpr auto lightEnd(Engine& engine_) const noexcept -> LightConstIterator {
			return this->lightBeforeBegin(engine_);
		}

		public: constexpr auto lightEnd(Engine& engine_) noexcept -> LightIterator {
			return this->lightBeforeBegin(engine_);
		}

		public: static constexpr auto fromLight(Engine& engine_, LightIterator lightIterator_) noexcept -> Iterator {
			return Iterator(lightIterator_, engine_);
		}

		public: static constexpr auto fromLight(Engine& engine_, LightConstIterator lightIterator_) noexcept -> ConstIterator {
			return Iterator(lightIterator_, engine_);
		}

		public: constexpr auto beforeBegin(Engine& engine_) const noexcept -> ConstIterator {
			return fromLight(engine_, this->lightBeforeBegin(engine_));
		}

		public: constexpr auto beforeBegin(Engine& engine_) noexcept -> Iterator {
			return fromLight(engine_, this->lightBeforeBegin(engine_));
		}

		public: constexpr auto begin(Engine& engine_) const noexcept -> ConstIterator {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool);
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

		private: template <typename... ArgumentTs__> static constexpr auto createNode_(
			Pool& pool_, ArgumentTs__&&... parameters_
		) -> Handle {
			Handle result_ = PoolAdaptorForNode_::allocate(pool_);
			try {
				Node* pointer_ = static_cast<Node*>(PoolAdaptorForNode_::handleConverter(pool_)(result_));
				new (pointer_) Node(::dcool::core::forward<ArgumentTs__>(parameters_)...);
			} catch (...) {
				PoolAdaptorForNode_::deallocate(pool_, result_);
				throw;
			}
			return result_;
		}

		private: template <typename... ArgumentTs__> static constexpr auto createNode_(
			Engine& engine_, ArgumentTs__&&... parameters_
		) -> Handle {
			return createNode_(engine_.pool, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		private: static constexpr void destroyNode_(Pool& pool_, Handle nodeHandle_) {
			HandleConverter converter_ = PoolAdaptorForNode_::handleConverter(pool_);
			auto node_ = static_cast<Node*>(converter_(nodeHandle_));
			node_->~Node();
			PoolAdaptorForNode_::deallocate(pool_, nodeHandle_);
		}

		private: static constexpr void destroyNode_(Engine& engine_, Handle nodeHandle_) {
			destroyNode_(engine_.pool, nodeHandle_);
		}

		private: constexpr auto insertNodeHeaderAfter_(
			HeaderHandleConverter const& headerConverter_, LightIterator position_, NodeHeader& headerToInsert_
		) noexcept -> LightIterator {
			NodeHeader& leading_ = position_.nodeHeader(headerConverter_);
			::dcool::core::detail_::insertForwardNodeHeaderAfter_(leading_, headerToInsert_, headerConverter_);
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
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(engine_.pool);
			return this->insertNodeAfter_(headerConverter_, position_, toInsert_);
		}

		// Note that 'toInsert_' must point to a node instead of a node header
		public: constexpr auto insertNodeAfter(
			Engine& engine_, LightIterator position_, Handle toInsert_
		) noexcept -> LightIterator {
			HandleConverter converter_ = PoolAdaptorForNode_::handleConverter(engine_.pool);
			return this->insertNodeAfter(engine_, position_, *static_cast<Node*>(converter_(toInsert_)));
		}

		public: constexpr auto insertNodeAfter(Iterator position_, Node& toInsert_) noexcept -> Iterator {
			return this->insertNodeAfter_(position_.converter(), static_cast<LightIterator>(position_), toInsert_);
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
				engine_, static_cast<LightIterator>(position_), ::dcool::core::forward<ArgumentTs__>(parameters_)...
			);
			return fromLight(engine_, result_);
		}

		public: template <typename... ArgumentTs__> constexpr void emplaceFront(Engine& engine_, ArgumentTs__&&... parameters_) {
			this->emplaceAfter(engine_, this->beforeBegin(engine_), ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr auto eraseAfter_(Pool& pool_, LightIterator positionBefore_) noexcept -> LightIterator {
			HeaderHandleConverter headerConverter_ = PoolAdaptorForNodeHeader_::handleConverter(pool_);
			HandleConverter converter_ = PoolAdaptorForNode_::handleConverter(pool_);
			NodeHeader& erasedNodeHeader_ = ::dcool::core::detail_::popForwardNodeHeaderAfter_(
				positionBefore_.nodeHeader(headerConverter_), headerConverter_
			);
			destroyNode_(pool_, converter_(static_cast<void*>(::dcool::core::addressOf(Node::retrieveFromHeader(erasedNodeHeader_)))));
			return positionBefore_.next(headerConverter_);
		}

		public: constexpr auto eraseAfter(Engine& engine_, LightIterator position_) noexcept -> LightIterator {
			return this->eraseAfter_(engine_.pool, position_);
		}

		public: constexpr auto eraseAfter(Engine& engine_, Iterator position_) noexcept -> Iterator {
			LightIterator result_ = this->eraseAfter_(engine_.pool, static_cast<LightIterator>(position_));
			return fromLight(engine_, result_);
		}
	};

	template <
		::dcool::core::Object ValueT_, ::dcool::core::ForwardLinkedConfigFor<ValueT_> ConfigT_ = ::dcool::core::Empty<>
	> struct ForwardLinked {
		private: using Self_ = ForwardLinked<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::core::ForwardLinkedConfigAdaptor<Value, Config>;
		public: using Chassis = ::dcool::core::ForwardLinkedChassis<Value, Config>;
		public: using Node = ConfigAdaptor_::ForwardLinkedNode;
		public: using NodeHeader = ConfigAdaptor_::ForwardLinkedNodeHeader;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Iterator = ConfigAdaptor_::ForwardLinkedIterator;
		public: using ConstIterator = ConfigAdaptor_::ForwardLinkedConstIterator;
		public: using LightIterator = ConfigAdaptor_::ForwardLinkedLightIterator;
		public: using LightConstIterator = ConfigAdaptor_::ForwardLinkedLightConstIterator;
		public: using SentryHolder = ConfigAdaptor_::SentryHolder;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean noexceptInitializeable = SentryHolder::noexceptInitializeable;

		private: [[no_unique_address]] mutable Engine m_engine_;
		private: Chassis m_chassis_;

		public: constexpr ForwardLinked() noexcept(Chassis::noexceptInitializeable) {
			this->chassis().initialize(this->engine());
		}

		public: constexpr ForwardLinked(Self_ const& other_): m_engine_(other_.m_engine_) {
			other_.chassis().cloneTo(other_.engine(), this->engine(), this->chassis());
		}

		public: constexpr ForwardLinked(Self_&& other_) noexcept(
			Chassis::noexceptInitializeable
		): m_engine_(::dcool::core::move(other_.m_engine_)) {
			other_.chassis().relocateTo(this->chassis());
			try {
				other_.initialize();
			} catch (...) {
				this->chassis().relocateTo(other_.chassis());
				throw;
			}
		}

		public: constexpr ~ForwardLinked() noexcept {
			this->chassis().uninitialize(this->engine());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(other_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) -> Self_& {
			this->engine() = other_.engine();
			other_.chassis().relocateTo(this->chassis());
			return *this;
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			::dcool::core::intelliswap(this->engine(), other_.engine());
			::dcool::core::intelliswap(this->chassis(), other_.chassis());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		public: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto lightBeforeBegin() const noexcept -> LightConstIterator {
			return this->chassis().lightBeforeBegin(this->engine());
		}

		public: constexpr auto lightBeforeBegin() noexcept -> LightIterator {
			return this->chassis().lightBeforeBegin(this->engine());
		}

		public: constexpr auto lightBegin() const noexcept -> LightConstIterator {
			return this->chassis().lightBegin(this->engine());
		}

		public: constexpr auto lightBegin() noexcept -> LightIterator {
			return this->chassis().lightBegin(this->engine());
		}

		public: constexpr auto lightEnd() const noexcept -> LightConstIterator {
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

		public: constexpr auto eraseAfter(LightIterator position_) noexcept -> LightIterator {
			return this->chassis().eraseAfter(this->engine(), position_);
		}

		public: constexpr auto eraseAfter(Iterator position_) noexcept -> Iterator {
			return this->chassis().eraseAfter(this->engine(), position_);
		}
	};
}

#endif
