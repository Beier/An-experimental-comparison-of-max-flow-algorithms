#pragma once

#include "tree.h"
#include "tree-inl.h"

class Forest {
 public:
  // don't change Cost to an unsigned type; that breaks the Min aggregate
  // read the appendix before changing Cost to a floating-point type
  typedef int Cost;

 private:
  typedef dtree::WithAncAggr<dtree::Min<Cost>,
                             dtree::WithAncValue<dtree::Add<Cost>,
                                                 dtree::Begin<> > > C;
  typedef dtree::WithEvert<C> E;
  typedef dtree::EndTree<E> Node;

 public:
  typedef size_t NodeId;
  Forest();
  ~Forest();
  // creates a forest with node ids 0..size-1
  void Initialize(size_t size);
  // as in [ST85]
  Cost FindCost(NodeId v) { return C::Value(&node_[v]); }
  NodeId FindRoot(NodeId v) { return Root(&node_[v]) - node_; }
  NodeId FindMin(NodeId v);
  void AddCost(NodeId v, Cost x) { C::AddToAnc(&node_[v], x); }
  void Link(NodeId v, NodeId w) { dtree::Link(&node_[v], &node_[w]); }
  void Cut(NodeId v) { dtree::Cut(&node_[v]); }
  void Evert(NodeId v) { E::Evert(&node_[v]); }

 private:
  Node* node_;
  // disallows the copy constructor and the assignment operator
  Forest(const Forest&);
  void operator=(const Forest&);
};

Forest::Forest() : node_(NULL) {
}

Forest::~Forest() {
  delete[] node_;
}

void Forest::Initialize(size_t size) {
  Node* old_node = node_;
  // avoids a double delete[] if new[] throws
  node_ = NULL;
  delete[] old_node;
  node_ = new Node[size];
}

Forest::NodeId Forest::FindMin(NodeId v) {
  return C::FindRootmostAnc(&node_[v],
                            dtree::LessEqual(C::AggrAnc(&node_[v]))) - node_;
}