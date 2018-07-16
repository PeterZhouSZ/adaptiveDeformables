#pragma once

#include <memory>
#include <queue>


template<typename Point, int Dimension>
class KDTree{
public:

  KDTree(const std::vector<Point>& points){
	
	std::vector<int > indices(points.size());
	std::iota(indices.begin(), indices.end(), 0);
	root = std::unique_ptr<Node<0> >(new Node<0>(points, indices.begin(), indices.end()));
  }

  KDTree(const std::vector<Point>& points, const std::vector<int>& indices){
	
	auto copiedIndices = indices;
	root = std::unique_ptr<Node<0> >(new Node<0>(points, copiedIndices.begin(), copiedIndices.end()));
  }

  
  std::vector<int> rangeQuery(const std::vector<Point>& points,
	  const Point& p, float radius) const{

	std::vector<int > ret;
	rangeQueryRecurse(root, points, p, radius, ret);
	return ret;
  }


  std::vector<int> KNN(const std::vector<Point>& points,
	  const Point& p, int k) const {

	std::vector<int > pQueue;
	knnRecurse(root, points, p, k, pQueue);
	return pQueue;
  }
  
private:


  static auto squaredDistance(const Point& a, const Point& b){
	auto d = (a[0] - b[0])*(a[0] - b[0]);
	for(auto i = 1; i < Dimension; ++i){
	  d += (a[i] - b[i])*(a[i] - b[i]);
	}
	return d;
  }
  
  template<int SplitDimension>
  struct CompareBy{
	CompareBy(const std::vector<Point>& _points)
	  :points(_points){}
	bool operator()(int a, int b) const{
	  auto xa = points[a][SplitDimension];
	  auto xb = points[b][SplitDimension];
	  
	  if(xa == xb){
		for(int i = 0; i < Dimension; ++i){
		  if(i != SplitDimension){
			if(points[a][i] != points[b][i]){
			  return points[a][i] < points[b][i];
			}
		  }
		}
	  }
	  return xa < xb;
	}
  private:
	const std::vector<Point>& points;
  };
  
  template<int SplitDimension>
  struct Node{
	int p; //index of P in points array
	std::unique_ptr<Node< (SplitDimension + 1)%Dimension> >  left, right;

	template<typename Iter>
	Node(const std::vector<Point>& points, Iter begin, Iter end)
	{
	  using ChildType = Node<(SplitDimension +1)%Dimension>;
	  
	  auto middle = begin + (end - begin)/2;
	  std::nth_element(begin, middle, end, CompareBy<SplitDimension>(points));
	  p = *middle;
	  if(begin != middle){
		left = std::unique_ptr<ChildType>(new ChildType(points, begin, middle));
	  }
	  if(middle +1 != end){
		right = std::unique_ptr<ChildType>(new ChildType(points, middle + 1, end));
	  }
	  
	}
	
  };

  struct DistanceComparator{
	DistanceComparator(const std::vector<Point>& _points, const Point& _p) : points(_points), p(_p){}
	bool operator()(int a, int b){
	  return squaredDistance(points[a], p) < squaredDistance(points[b], p);
	}
  private:
	Point p;
	const std::vector<Point>& points;
  };
  
  std::unique_ptr<Node<0> > root;


  template<int SplitDimension>
  void rangeQueryRecurse(const std::unique_ptr<Node<SplitDimension> >& node,
	  const std::vector<Point>& points, const Point& p, float radius,
	  std::vector<int >& ret) const{

	if(squaredDistance(p, points[node->p]) < radius*radius){
	  ret.push_back(node->p);
	}

	if(node->left && points[node->p][SplitDimension] >= (p[SplitDimension] - radius)){
	  rangeQueryRecurse(node->left, points, p, radius, ret);
	}

	if(node->right && points[node->p][SplitDimension] <= (p[SplitDimension] + radius)){
	  rangeQueryRecurse(node->right, points, p, radius, ret);
	}
	

  }

  template <int SplitDimension>
  void knnRecurse(const std::unique_ptr<Node<SplitDimension> >& node,
	  const std::vector<Point>& points, const Point& p,
	  int k, std::vector<int>& pQueue) const{ 

	using ChildType = Node<(SplitDimension +1)%Dimension>;
	DistanceComparator cmp{points, p};
	
	if(pQueue.size() < k){

	  pQueue.push_back(node->p);
	  std::push_heap(pQueue.begin(), pQueue.end(), cmp);
	  
	} else if (squaredDistance(points[node->p], p) < squaredDistance(points[pQueue.front()], p)){

	  std::pop_heap(pQueue.begin(), pQueue.end(), cmp);
	  pQueue.pop_back();
	  pQueue.push_back(node->p);
	  std::push_heap(pQueue.begin(), pQueue.end(), cmp);
	  
	}
	
	if(node->left){
	  auto radius = std::sqrt(squaredDistance(points[pQueue.front()], p));
	  if(pQueue.size() < k || points[node->p][SplitDimension] >= (p[SplitDimension] - radius)){
		knnRecurse(node->left, points, p, k, pQueue);
	  }
	}

	if(node->right){
	  auto radius = std::sqrt(squaredDistance(points[pQueue.front()], p));	  
	  if(pQueue.size() < k || points[node->p][SplitDimension] <= (p[SplitDimension] + radius)){
		knnRecurse(node->right, points, p, k, pQueue);
	  }
	}
	
  }
};

