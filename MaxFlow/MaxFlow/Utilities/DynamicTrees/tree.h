// -*-C++-*-
// Copyright (c) 2012 David Eisenstat <eisenstatdavid@gmail.com>
// Released under the terms of license.txt ("The MIT License")
// July 2012 version

#ifndef DTREE_TREE_H_
#define DTREE_TREE_H_

#include "seq.h"

namespace dtree {

template<typename Base>
class EndTree : public Base {
 public:
  EndTree() : parent_(NULL), solid_() {}
  EndTree* parent() const { return parent_; }
  void set_parent(EndTree* parent) { parent_ = parent; }
  EndTree* solid(int i) const { return solid_[i]; }
  void set_solid(int i, EndTree* solid_i) { solid_[i] = solid_i; }
  template<typename Forwarder>
  void CopyFrom(const EndTree& node, Forwarder forward);
  void SolidResetAggrs() { Base::SolidResetAggrs(NULL); }
  bool SolidIndex(const EndTree* parent, int* i) const {
    if (!parent) return false;
    if (parent->solid(0) == this) {
      *i = 0;
      return true;
    }
    if (parent->solid(1) == this) {
      *i = 1;
      return true;
    }
    return false;
  }
  template<bool finishing_search>
  void ExposeStep(EndTree** d0, EndTree** d1);
  template<bool finishing_search>
  void DashedReplace(EndTree* c, EndTree* d0, EndTree* d1);
  template<bool finishing_search>
  void DashedRemove(EndTree* d0, EndTree* d1);
  void DashedInsert(EndTree* c);
  EndTree* Expose();
  EndTree* HardExpose();
  void FinishSearch();
  //
  size_t num_desc() const { return abstract_.num_desc; }
  void set_num_desc(size_t num_desc) { abstract_.num_desc = num_desc; }
  EndTree* heavy() const { return abstract_.heavy; }
  void set_heavy(EndTree* heavy) { abstract_.heavy = heavy; }
  void ResetDashed();
  void ResetDotted();
  void AssembleDashed(EndTree* path[]);
  void AssembleSolidRoot(EndTree* parent);

 private:
  EndTree* parent_;
  union {
    EndTree* solid_[2];
    struct {
      size_t num_desc;
      EndTree* heavy;
    } abstract_;
  };
  EndTree(const EndTree&);
  void operator=(const EndTree&);
};

template<typename Base>
class EndTreeWithDesc : public Base {
 public:
  struct DottedPrimitives;
  EndTreeWithDesc() : parent_(NULL), dashed_(NULL), solid_(), dotted_() {}
  EndTreeWithDesc* parent() const { return parent_; }
  void set_parent(EndTreeWithDesc* parent) { parent_ = parent; }
  EndTreeWithDesc* dashed() const { return dashed_; }
  void set_dashed(EndTreeWithDesc* dashed) { dashed_ = dashed; }
  EndTreeWithDesc* solid(int i) const { return solid_[i]; }
  void set_solid(int i, EndTreeWithDesc* solid_i) { solid_[i] = solid_i; }
  EndTreeWithDesc* dotted(int i) const { return dotted_[i]; }
  void set_dotted(int i, EndTreeWithDesc* dotted_i) { dotted_[i] = dotted_i; }
  template<typename Forwarder>
  void CopyFrom(const EndTreeWithDesc& node, Forwarder forward);
  void SolidResetAggrs() { Base::SolidResetAggrs(dashed()); }
  bool SolidIndex(const EndTreeWithDesc* parent, int* i) const {
    if (!parent) return false;
    if (parent->solid(0) == this) {
      *i = 0;
      return true;
    }
    if (parent->solid(1) == this) {
      *i = 1;
      return true;
    }
    return false;
  }
  void SolidToDashed(const EndTreeWithDesc* parent) {
    Base::SolidToDashed(parent);
    Base::SolidToDashedAggrs();
  }
  void DottedResetAggrs() {
    Base::DottedResetAggrs(dashed());
    if (EndTreeWithDesc* c = solid(0)) this->DottedMergeSolidAggrs(c);
    if (EndTreeWithDesc* e = solid(1)) this->DottedMergeSolidAggrs(e);
  }
  bool DottedIndex(const EndTreeWithDesc* parent, int* i) const {
    if (parent->dotted(0) == this) {
      *i = 0;
      return true;
    }
    if (parent->dotted(1) == this) {
      *i = 1;
      return true;
    }
    return false;
  }
  template<bool finishing_search>
  void ExposeStep(EndTreeWithDesc** d0, EndTreeWithDesc** d1);
  template<bool finishing_search>
  void DashedReplace(EndTreeWithDesc* c,
                     EndTreeWithDesc* d0,
                     EndTreeWithDesc* d1);
  template<bool finishing_search>
  void DashedRemove(EndTreeWithDesc* d0, EndTreeWithDesc* d1);
  void DashedInsert(EndTreeWithDesc* c);
  EndTreeWithDesc* Expose();
  EndTreeWithDesc* HardExpose();
  void FinishSearch();
  EndTreeWithDesc* DottedCut(int i);
  void DottedLink(int i, EndTreeWithDesc* dotted_i);
  template<bool finishing_search>
  void DottedSplay(EndTreeWithDesc** d0, EndTreeWithDesc** d1);
  void Splice(EndTreeWithDesc* d);
  void AbsorbDashed(EndTreeWithDesc* v);
  //
  size_t num_desc() const { return abstract_.num_desc; }
  void set_num_desc(size_t num_desc) { abstract_.num_desc = num_desc; }
  EndTreeWithDesc* heavy() const { return abstract_.heavy; }
  void set_heavy(EndTreeWithDesc* heavy) { abstract_.heavy = heavy; }
  void ResetDashed();
  void ResetDotted();
  void AssembleDashed(EndTreeWithDesc* path[]);
  void AssembleSolidRoot(EndTreeWithDesc* parent);
  void SolidToDashedAggrs(EndTreeWithDesc* path[]);
  void nop() {}

 private:
  EndTreeWithDesc* parent_;
  EndTreeWithDesc* dashed_;
  EndTreeWithDesc* solid_[2];
  union {
    EndTreeWithDesc* dotted_[2];
    struct {
      size_t num_desc;
      EndTreeWithDesc* heavy;
    } abstract_;
  };
  EndTreeWithDesc(const EndTreeWithDesc&);
  void operator=(const EndTreeWithDesc&);
};

template<typename Group, typename Base>
class WithAncValue : public WithValue<Group, Base> {
 protected:
  WithAncValue() {}
};

template<typename Group, typename Base>
class WithDescValue : public Base {
 public:
  typedef Group Group_;
  enum { kValueScope = kDesc };
  typename Group::Type value() const { return value_; }
  void set_value(typename Group::Type value) { value_ = value; }
  void add_to_value(typename Group::Type delta) {
    set_value(Group::Plus(value(), delta));
  }
  void subtract_from_value(typename Group::Type delta) {
    set_value(Group::Minus(value(), delta));
  }
  void SolidAddValues(const WithDescValue* parent) {
    Base::SolidAddValues(parent);
    add_to_value(parent->value());
  }
  void SolidSubtractValues(const WithDescValue* parent) {
    Base::SolidSubtractValues(parent);
    subtract_from_value(parent->value());
  }
  void DottedAddValues(const WithDescValue* parent) {
    Base::DottedAddValues(parent);
    add_to_value(parent->value());
  }
  void DottedSubtractValues(const WithDescValue* parent) {
    Base::DottedSubtractValues(parent);
    subtract_from_value(parent->value());
  }
  //
  template<typename Node>
  static typename Group::Type Value(Node* node);
  template<typename Node>
  static void SetValue(Node* node, typename Group::Type value);
  template<typename Node>
  static void AddToTree(Node* node, typename Group::Type delta);
  template<typename Node>
  static void SubtractFromTree(Node* node, typename Group::Type delta);
  template<typename Node>
  static void AddToDesc(Node* node, typename Group::Type delta);
  template<typename Node>
  static void SubtractFromDesc(Node* node, typename Group::Type delta);
  template<typename Node>
  static void AddToProperDesc(Node* node, typename Group::Type delta);
  template<typename Node>
  static void SubtractFromProperDesc(Node* node, typename Group::Type delta);

 protected:
  WithDescValue() : value_() {}

 private:
  static void AddToSeq();
  static void SubtractFromSeq();
  static void AddToAnc();
  static void SubtractFromAnc();
  static void AddToProperAnc();
  static void SubtractFromProperAnc();
  static void AggrSeq();
  static void FindDirmostSeq();
  static void FindLeftmostSeq();
  static void FindRightmostSeq();
  static void AggrAnc();
  static void FindDirmostAnc();
  static void FindLeafmostAnc();
  static void FindRootmostAnc();
  static void AggrProperAnc();
  static void FindDirmostProperAnc();
  static void FindLeafmostProperAnc();
  static void FindRootmostProperAnc();
  static void AggrTree();
  static void FindDirmostTree();
  static void FindLeafmostTree();
  static void FindRootmostTree();
  static void AggrDesc();
  static void FindDirmostDesc();
  static void FindLeafmostDesc();
  static void FindRootmostDesc();
  static void AggrProperDesc();
  static void FindDirmostProperDesc();
  static void FindLeafmostProperDesc();
  static void FindRootmostProperDesc();
  static void ReverseSeq();
  static void Evert();
  typename Group::Type value_;
};

template<typename Semigroup, typename Base>
class WithAncAggr : public WithAggr<Semigroup, Base> {
 protected:
  WithAncAggr() {}
 private:
};

template<typename Semigroup, typename Base>
class WithDescAggrOfDescValue : public Base {
 private:
  typedef typename Base::Group_ Group;

 public:
  typedef Semigroup Semigroup_;
  enum { kAggrScope = kDesc };
  typename Semigroup::Type delta_aggr() const { return delta_aggr_; }
  void set_delta_aggr(typename Semigroup::Type delta_aggr) {
    delta_aggr_ = delta_aggr;
  }
  typename Semigroup::Type singleton_aggr() const {
    return Semigroup::AggrFromValue(this->value());
  }
  typename Semigroup::Type aggr() const {
    return Group::Plus(delta_aggr(), this->value());
  }
  void SolidResetAggrs(const WithDescAggrOfDescValue* dashed) {
    Base::SolidResetAggrs(dashed);
    Reset1(dashed);
  }
  void SolidMergeSolidAggrs(const WithDescAggrOfDescValue* solid) {
    Base::SolidMergeSolidAggrs(solid);
    Merge1(solid);
  }
  void DottedResetAggrs(const WithDescAggrOfDescValue* dashed) {
    Base::DottedResetAggrs(dashed);
    Reset1(dashed);
  }
  void DottedMergeSolidAggrs(const WithDescAggrOfDescValue* solid) {
    Base::DottedMergeSolidAggrs(solid);
    Merge1(solid);
  }
  void DottedMergeDottedAggrs(const WithDescAggrOfDescValue* dotted) {
    Base::DottedMergeDottedAggrs(dotted);
    Merge1(dotted);
  }
  //
  template<typename Node>
  static typename Semigroup::Type AggrTree(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostTree(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostTree(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostTree(Node* node, Predicate predicate);
  template<typename Node>
  static typename Semigroup::Type AggrDesc(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostDesc(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostDesc(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostDesc(Node* node, Predicate predicate);
  template<typename Node>
  static typename Semigroup::Type AggrProperDesc(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostProperDesc(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostProperDesc(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostProperDesc(Node* node, Predicate predicate);

 protected:
  WithDescAggrOfDescValue() : delta_aggr_() {}

 private:
  template<typename Node, typename Predicate>
  class DescSearch;
  void Reset1(const WithDescAggrOfDescValue* dashed) {
    set_delta_aggr(Group::Minus(singleton_aggr(), this->value()));
    if (dashed) Merge1(dashed);
  }
  void Merge1(const WithDescAggrOfDescValue* child) {
    set_delta_aggr(Semigroup::CombineAggrs(delta_aggr(), child->aggr()));
  }
  static void AggrSeq();
  static void FindDirmostSeq();
  static void FindLeftmostSeq();
  static void FindRightmostSeq();
  static void AggrAnc();
  static void FindDirmostAnc();
  static void FindLeafmostAnc();
  static void FindRootmostAnc();
  static void AggrProperAnc();
  static void FindDirmostProperAnc();
  static void FindLeafmostProperAnc();
  static void FindRootmostProperAnc();
  typename Semigroup::Type delta_aggr_;
};

template<typename Semigroup, typename Base>
class WithDescAggrOfAncValue : public WithAggr<Semigroup, Base> {
 public:
  enum { kAggrScope = kDesc };
  typename Semigroup::Type other_aggr() const { return other_aggr_; }
  void set_other_aggr(typename Semigroup::Type other_aggr) {
    other_aggr_ = other_aggr;
  }
  typename Semigroup::Type combined_aggr() const {
    return Semigroup::CombineAggrs(this->aggr(), other_aggr());
  }
  void SolidResetAggrs(const WithDescAggrOfAncValue* dashed) {
    typedef WithAggr<Semigroup, Base> Base1;
    Base1::SolidResetAggrs(dashed);
    set_other_aggr(dashed ? dashed->other_aggr() : Semigroup::empty_aggr());
  }
  void SolidMergeSolidAggrs(const WithDescAggrOfAncValue* solid) {
    typedef WithAggr<Semigroup, Base> Base1;
    Base1::SolidMergeSolidAggrs(solid);
    Merge1(solid);
  }
  void SolidToDashedAggrs() {
    typedef WithAggr<Semigroup, Base> Base1;
    Base1::SolidToDashedAggrs();
    this->set_delta_aggr(combined_aggr());
  }
  void DottedResetAggrs(const WithDescAggrOfAncValue* dashed) {
    typedef WithAggr<Semigroup, Base> Base1;
    Base1::DottedResetAggrs(dashed);
    set_other_aggr(this->delta_aggr());
  }
  void DottedMergeDottedAggrs(const WithDescAggrOfAncValue* dotted) {
    typedef WithAggr<Semigroup, Base> Base1;
    Base1::DottedMergeDottedAggrs(dotted);
    Merge1(dotted);
  }
  //
  template<typename Node>
  static typename Semigroup::Type AggrTree(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostTree(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostTree(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostTree(Node* node, Predicate predicate);
  template<typename Node>
  static typename Semigroup::Type AggrDesc(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostDesc(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostDesc(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostDesc(Node* node, Predicate predicate);
  template<typename Node>
  static typename Semigroup::Type AggrProperDesc(Node* node);
  template<typename Node, typename Predicate>
  static Node* FindDirmostProperDesc(int dir, Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindLeafmostProperDesc(Node* node, Predicate predicate);
  template<typename Node, typename Predicate>
  static Node* FindRootmostProperDesc(Node* node, Predicate predicate);

 protected:
  WithDescAggrOfAncValue() : other_aggr_(Semigroup::empty_aggr()) {}

 private:
  template<typename Node, typename Predicate>
  class DescSearch;
  void Merge1(const WithDescAggrOfAncValue* child) {
    set_other_aggr(Semigroup::CombineAggrs(other_aggr(), child->other_aggr()));
  }
  typename Semigroup::Type other_aggr_;
};

template<int value_scope>
struct WdaSelector;

template<>
struct WdaSelector<kDesc> {
  template<typename Semigroup, typename Base>
  class Wda : public WithDescAggrOfDescValue<Semigroup, Base> {
   protected:
    Wda() {}
  };
};

template<>
struct WdaSelector<kAnc> {
  template<typename Semigroup, typename Base>
  class Wda : public WithDescAggrOfAncValue<Semigroup, Base> {
   protected:
    Wda() {}
  };
};

template<typename Semigroup, typename Base>
class WithDescAggr
    : public WdaSelector<Base::kValueScope>::template Wda<Semigroup, Base> {
 protected:
  WithDescAggr() {}
};

template<typename Group, typename Base>
class WithEvertBy : public WithReverseBy<Group, Base> {
 protected:
  WithEvertBy() {}
};

template<typename Base>
class WithEvert : public WithReverse<Base> {
 protected:
  WithEvert() {}
};
}  // namespace dtree
#endif  // DTREE_TREE_H_
