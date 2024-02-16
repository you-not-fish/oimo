// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: test.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_test_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_test_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_test_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_test_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_test_2eproto;
namespace test {
class TestMessage;
struct TestMessageDefaultTypeInternal;
extern TestMessageDefaultTypeInternal _TestMessage_default_instance_;
}  // namespace test
PROTOBUF_NAMESPACE_OPEN
template<> ::test::TestMessage* Arena::CreateMaybeMessage<::test::TestMessage>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace test {

// ===================================================================

class TestMessage final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:test.TestMessage) */ {
 public:
  inline TestMessage() : TestMessage(nullptr) {}
  ~TestMessage() override;
  explicit PROTOBUF_CONSTEXPR TestMessage(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  TestMessage(const TestMessage& from);
  TestMessage(TestMessage&& from) noexcept
    : TestMessage() {
    *this = ::std::move(from);
  }

  inline TestMessage& operator=(const TestMessage& from) {
    CopyFrom(from);
    return *this;
  }
  inline TestMessage& operator=(TestMessage&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const TestMessage& default_instance() {
    return *internal_default_instance();
  }
  static inline const TestMessage* internal_default_instance() {
    return reinterpret_cast<const TestMessage*>(
               &_TestMessage_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(TestMessage& a, TestMessage& b) {
    a.Swap(&b);
  }
  inline void Swap(TestMessage* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(TestMessage* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  TestMessage* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<TestMessage>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const TestMessage& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const TestMessage& from) {
    TestMessage::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(TestMessage* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "test.TestMessage";
  }
  protected:
  explicit TestMessage(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kEmailFieldNumber = 3,
    kNameFieldNumber = 1,
    kIdFieldNumber = 2,
  };
  // repeated string email = 3;
  int email_size() const;
  private:
  int _internal_email_size() const;
  public:
  void clear_email();
  const std::string& email(int index) const;
  std::string* mutable_email(int index);
  void set_email(int index, const std::string& value);
  void set_email(int index, std::string&& value);
  void set_email(int index, const char* value);
  void set_email(int index, const char* value, size_t size);
  std::string* add_email();
  void add_email(const std::string& value);
  void add_email(std::string&& value);
  void add_email(const char* value);
  void add_email(const char* value, size_t size);
  const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>& email() const;
  ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>* mutable_email();
  private:
  const std::string& _internal_email(int index) const;
  std::string* _internal_add_email();
  public:

  // string name = 1;
  void clear_name();
  const std::string& name() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_name(ArgT0&& arg0, ArgT... args);
  std::string* mutable_name();
  PROTOBUF_NODISCARD std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // int32 id = 2;
  void clear_id();
  int32_t id() const;
  void set_id(int32_t value);
  private:
  int32_t _internal_id() const;
  void _internal_set_id(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:test.TestMessage)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string> email_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
    int32_t id_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_test_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// TestMessage

// string name = 1;
inline void TestMessage::clear_name() {
  _impl_.name_.ClearToEmpty();
}
inline const std::string& TestMessage::name() const {
  // @@protoc_insertion_point(field_get:test.TestMessage.name)
  return _internal_name();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void TestMessage::set_name(ArgT0&& arg0, ArgT... args) {
 
 _impl_.name_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:test.TestMessage.name)
}
inline std::string* TestMessage::mutable_name() {
  std::string* _s = _internal_mutable_name();
  // @@protoc_insertion_point(field_mutable:test.TestMessage.name)
  return _s;
}
inline const std::string& TestMessage::_internal_name() const {
  return _impl_.name_.Get();
}
inline void TestMessage::_internal_set_name(const std::string& value) {
  
  _impl_.name_.Set(value, GetArenaForAllocation());
}
inline std::string* TestMessage::_internal_mutable_name() {
  
  return _impl_.name_.Mutable(GetArenaForAllocation());
}
inline std::string* TestMessage::release_name() {
  // @@protoc_insertion_point(field_release:test.TestMessage.name)
  return _impl_.name_.Release();
}
inline void TestMessage::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  _impl_.name_.SetAllocated(name, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.name_.IsDefault()) {
    _impl_.name_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:test.TestMessage.name)
}

// int32 id = 2;
inline void TestMessage::clear_id() {
  _impl_.id_ = 0;
}
inline int32_t TestMessage::_internal_id() const {
  return _impl_.id_;
}
inline int32_t TestMessage::id() const {
  // @@protoc_insertion_point(field_get:test.TestMessage.id)
  return _internal_id();
}
inline void TestMessage::_internal_set_id(int32_t value) {
  
  _impl_.id_ = value;
}
inline void TestMessage::set_id(int32_t value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:test.TestMessage.id)
}

// repeated string email = 3;
inline int TestMessage::_internal_email_size() const {
  return _impl_.email_.size();
}
inline int TestMessage::email_size() const {
  return _internal_email_size();
}
inline void TestMessage::clear_email() {
  _impl_.email_.Clear();
}
inline std::string* TestMessage::add_email() {
  std::string* _s = _internal_add_email();
  // @@protoc_insertion_point(field_add_mutable:test.TestMessage.email)
  return _s;
}
inline const std::string& TestMessage::_internal_email(int index) const {
  return _impl_.email_.Get(index);
}
inline const std::string& TestMessage::email(int index) const {
  // @@protoc_insertion_point(field_get:test.TestMessage.email)
  return _internal_email(index);
}
inline std::string* TestMessage::mutable_email(int index) {
  // @@protoc_insertion_point(field_mutable:test.TestMessage.email)
  return _impl_.email_.Mutable(index);
}
inline void TestMessage::set_email(int index, const std::string& value) {
  _impl_.email_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set:test.TestMessage.email)
}
inline void TestMessage::set_email(int index, std::string&& value) {
  _impl_.email_.Mutable(index)->assign(std::move(value));
  // @@protoc_insertion_point(field_set:test.TestMessage.email)
}
inline void TestMessage::set_email(int index, const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.email_.Mutable(index)->assign(value);
  // @@protoc_insertion_point(field_set_char:test.TestMessage.email)
}
inline void TestMessage::set_email(int index, const char* value, size_t size) {
  _impl_.email_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:test.TestMessage.email)
}
inline std::string* TestMessage::_internal_add_email() {
  return _impl_.email_.Add();
}
inline void TestMessage::add_email(const std::string& value) {
  _impl_.email_.Add()->assign(value);
  // @@protoc_insertion_point(field_add:test.TestMessage.email)
}
inline void TestMessage::add_email(std::string&& value) {
  _impl_.email_.Add(std::move(value));
  // @@protoc_insertion_point(field_add:test.TestMessage.email)
}
inline void TestMessage::add_email(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _impl_.email_.Add()->assign(value);
  // @@protoc_insertion_point(field_add_char:test.TestMessage.email)
}
inline void TestMessage::add_email(const char* value, size_t size) {
  _impl_.email_.Add()->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_add_pointer:test.TestMessage.email)
}
inline const ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>&
TestMessage::email() const {
  // @@protoc_insertion_point(field_list:test.TestMessage.email)
  return _impl_.email_;
}
inline ::PROTOBUF_NAMESPACE_ID::RepeatedPtrField<std::string>*
TestMessage::mutable_email() {
  // @@protoc_insertion_point(field_mutable_list:test.TestMessage.email)
  return &_impl_.email_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace test

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_test_2eproto