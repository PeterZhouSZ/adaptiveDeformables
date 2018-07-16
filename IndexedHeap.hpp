#pragma once

#include <vector>
#include <algorithm>


//supports heap operations (push, pop)
//as well as random access operations

template <typename Elem>
class IndexedHeap{
public:
  IndexedHeap(const std::vector<Elem>& _elements)
	:elements(_elements), heap(elements.size()), heapPositions(elements.size())
  {
	std::iota(heap.begin(), heap.end(), 0);
	std::iota(heapPositions.begin(), heapPositions.end(), 0);
	heapify();

  }
  

  IndexedHeap(const std::vector<Elem>& _elements, const std::vector<int>& indices)
	:elements(_elements), heap(indices), heapPositions(_elements.size(), _elements.size())
	 //initialize all positions to OOB
  {
	for(int i = 0; i < indices.size(); ++i){
	  heapPositions[indices[i]] = i;
	}
	
	heapify();

  }

  
  int popMax(){
	swap(0, heap.size() -1);
	int ret = heap.back();
	heap.pop_back();
	pushDown(0);
	return ret;
  }

  //returns true if the element is still in the heap
  //it may have gotten popped.
  bool elementReweighted(int index){
	auto heapSpot = heapPositions[index];
	if(heapSpot < heap.size()){
	  updatePosition(heapSpot);
	  return true;
	}
	return false;
  }

  int size() const{
	return heap.size();
  }

  std::vector<int> extractVector() &&{
	return heap;
  }
  
private:
  const std::vector<Elem>& elements;
  std::vector<int> heap, heapPositions;

  //swap elements at indices a and b in the heap
  //update the heapPositions entry to maintain index ops
  void swap(int a, int b){
	std::swap(heap[a], heap[b]);
	heapPositions[heap[a]] = a;
	heapPositions[heap[b]] = b;

	
  }

  static constexpr int leftChild(int i){ return (i+1)*2 - 1; }
  static constexpr int rightChild(int i){ return (i+1)*2; }
  static constexpr int parent(int i){ return (i -1)/2; }

  
  void heapify(int start){
	int largest = start;
	if(leftChild(start) < heap.size() && elements[heap[leftChild(start)]] > elements[heap[largest]]){
	  largest = leftChild(start);
	}
	if(rightChild(start) < heap.size() && elements[heap[rightChild(start)]] > elements[heap[largest]]){
	  largest = rightChild(start);
	}
	if(largest != start){
	  swap(largest, start);
	  heapify(largest);
	}
  }


  void heapify(){
	for(int i = heap.size()/2; i >= 0; --i){
	  heapify(i);
	}
  }

  //return true if it moved.
  bool pushDown(int i){
	int start = i;
	while(leftChild(i) < heap.size()){ //check children
	  
	  int largerChild = leftChild(i);
	  if(rightChild(i) < heap.size() && elements[heap[leftChild(i)]] < elements[heap[rightChild(i)]]){
		largerChild = rightChild(i);
	  }

	  if(elements[heap[i]] >= elements[heap[largerChild]]){
		return i != start;
	  }
	  
	  swap(i, largerChild);
	  i = largerChild;
	}
	return i != start;
  }

  //true if i moved
  bool pushUp(int i){
	int start = i;
	while(parent(i) >= 0 && parent(i) != i){ //-1/2 == 0 in C, so double check
	  if(elements[heap[parent(i)]] < elements[heap[i]]){
		swap(i, parent(i));
		i = parent(i);
	  } else {
		break;
	  }
	}
	return i != start;
  }

  //true if i moved
  bool updatePosition(int i){
	if(!pushUp(i)){
	  return pushDown(i);
	}
	return true; //we pushed up
  }

  void checkRep(int i) const{
	if(leftChild(i) < heap.size()){
	  assert(elements[heap[i]] >= elements[heap[leftChild(i)]]);
	  checkRep(leftChild(i));
	}
	if(rightChild(i) < heap.size()){
	  assert(elements[heap[i]] >= elements[heap[rightChild(i)]]);
	  checkRep(rightChild(i));
	}
  }

  
};
