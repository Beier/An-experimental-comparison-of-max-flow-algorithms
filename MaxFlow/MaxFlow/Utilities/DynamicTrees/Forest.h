#pragma once

#include "tree.h"
#include "tree-inl.h"
#include <vector>
#include <iostream>


template<typename NodeBase>
class Forest {
 public:
  // don't change Cost to an unsigned type; that breaks the Min aggregate
  // read the appendix before changing Cost to a floating-point type
  typedef int Cost;

 private:
struct N : public NodeBase
{
	int Count;
};
  typedef dtree::WithAncAggr<dtree::Min<Cost>,
                             dtree::WithAncValue<dtree::Add<Cost>,
                                                 dtree::Begin<N> > > C;
  typedef dtree::WithEvert<C> E;
  typedef dtree::EndTree<E> Node;

 public:
  typedef size_t NodeId;

 Forest() : node_(NULL) {
}

~Forest() {
  delete[] node_;
}

void Initialize(size_t size) {
  Node* old_node = node_;
  // avoids a double delete[] if new[] throws
  node_ = NULL;
  delete[] old_node;
  node_ = new Node[size];
  for (int i = 0; i < size; i++)
	  node_[i].Count = 1;
}

NodeId FindMin(NodeId v) {
  return C::FindRootmostAnc(&node_[v],
                            dtree::LessEqual(C::AggrAnc(&node_[v]))) - node_;
}

NodeId FindAncLessThan(NodeId v, int value) {
  return C::FindLeafmostAnc(&node_[v],
                            dtree::Less(value)) - node_;
}
  // creates a forest with node ids 0..size-1
  // as in [ST85]
  Cost FindCost(NodeId v) { return C::Value(&node_[v]); }
  void SetCost (NodeId v, int value) {C::SetValue(&node_[v], value);}
  NodeId FindRoot(NodeId v) { return Root(&node_[v]) - node_; }
  NodeId GetId(Node* v){return v-node_;}
  void AddCost(NodeId v, Cost x) { C::AddToAnc(&node_[v], x); }
  bool Link(NodeId v, NodeId w) 
  { 
	  Node* nodeV = &node_[v];
	  Node* nodeW = &node_[w];
	  Node* root = Root(nodeV);

	  if(root != nodeV) //V is not a root
		  return false;
	  if(Root(nodeW) == nodeV) //V and W is in the same tree
		  return false;

	  dtree::Link(nodeV, nodeW); 

	  int size = nodeV->Count;
	  
	  while (nodeV != root)
	  {
		  nodeV = dtree::Parent(nodeV);
		  nodeV->Count += size;
	  }
	  return true;
  }

  int GetChild(NodeId v)
  {
	  Node* node = &node_[v];
	  Node* foundNode = dtree::Child(node);
	  
	  if(foundNode == NULL)
		  return -1;
	  return GetId(foundNode);
  }

  int GetParentId(NodeId v)
  {
	  Node* node = dtree::Parent((&node_[v]));
	  if (!node)
		return -1;
	  return GetId(node);
  }

  bool Cut(NodeId v) 
  {
	  Node* node = &node_[v];

	  int size = node->Count;
	  Node* root = Root(node);
	  if(root == node)	//trying to cut a root
		  return false;
	  while (node != root)
	  {
		  node = dtree::Parent(node);
		  node->Count -= size;
	  }
	  dtree::Cut(&node_[v]); 
	  return true;
  }

  //void Evert(NodeId v) { E::Evert(&node_[v]); }
  int GetTreeSize(NodeId v)
  {
	  return Root(&node_[v])->Count;
  }

  int GetSize(NodeId v)
  {
	  return (&node_[v])->Count;
  }


  Node* GetNode(int index){return &node_[index];}



 private:
  Node* node_;
  // disallows the copy constructor and the assignment operator
  Forest(const Forest&);
  void operator=(const Forest&);
};



template<typename NodeBase>
class ForestWithChildPointers {
public:
	// don't change Cost to an unsigned type; that breaks the Min aggregate
	// read the appendix before changing Cost to a floating-point type
	typedef int Cost;

private:
	struct N : public NodeBase
	{
		int Count;
	};
	typedef dtree::WithAggr<dtree::Min<Cost>,
		dtree::WithValue<dtree::Add<Cost>,
		dtree::Begin<N> > > C;  
	typedef dtree::EndTreeWithDesc<C> Node; 

public:
	typedef size_t NodeId;

	ForestWithChildPointers() : node_(NULL) {
	}

	~ForestWithChildPointers() {
		delete[] node_;
	}

	void Initialize(size_t size) {
		Node* old_node = node_;
		// avoids a double delete[] if new[] throws
		node_ = NULL;
		delete[] old_node;
		node_ = new Node[size];
		for (int i = 0; i < size; i++)
			node_[i].Count = 1;
	}

	NodeId FindMin(NodeId v) {
		return C::FindRootmostAnc(&node_[v],
			dtree::LessEqual(C::AggrAnc(&node_[v]))) - node_;
	}
	// creates a forest with node ids 0..size-1
	// as in [ST85]
	Cost FindCost(NodeId v) { return C::Value(&node_[v]); }
	void SetCost (NodeId v, int value) {C::SetValue(&node_[v], value);}
	NodeId FindRoot(NodeId v) { return Root(&node_[v]) - node_; }
	NodeId GetId(Node* v){return v-node_;}
	void AddCost(NodeId v, Cost x) { C::AddToAnc(&node_[v], x); }
	bool Link(NodeId v, NodeId w) 
	{ 
		Node* nodeV = &node_[v];
		Node* nodeW = &node_[w];
		Node* root = Root(nodeV);

		if(root != nodeV) //V is not a root
			return false;
		if(Root(nodeW) == nodeV) //V and W is in the same tree
			return false;

		dtree::Link(nodeV, nodeW); 

		int size = nodeV->Count;

		while (nodeV != root)
		{
			nodeV = dtree::Parent(nodeV);
			nodeV->Count += size;
		}
		return true;
	}

	int GetChild(NodeId v)
	{
		Node* node = &node_[v];
		Node* foundNode = dtree::Child(node);

		if(foundNode == NULL)
			return -1;
		return GetId(foundNode);
	}

	int GetParentId(NodeId v)
	{
		Node* node = dtree::Parent((&node_[v]));
		if (!node)
			return -1;
		return GetId(node);
	}

	bool Cut(NodeId v) 
	{
		Node* node = &node_[v];

		int size = node->Count;
		Node* root = Root(node);
		if(root == node)	//trying to cut a root
			return false;
		while (node != root)
		{
			node = dtree::Parent(node);
			node->Count -= size;
		}
		dtree::Cut(&node_[v]); 
		return true;
	}

  //void Evert(NodeId v) { E::Evert(&node_[v]); }
  int GetTreeSize(NodeId v)
  {
	  return Root(&node_[v])->Count;
  }

  int GetSize(NodeId v)
  {
	  return (&node_[v])->Count;
  }


  Node* GetNode(int index){return &node_[index];}



 private:
  Node* node_;
  // disallows the copy constructor and the assignment operator
  ForestWithChildPointers(const ForestWithChildPointers&);
  void operator=(const ForestWithChildPointers&);
};