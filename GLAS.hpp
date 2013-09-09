#pragma once
#include <quaternion.h>
#include <iterator>
#include <boost\iterator\iterator_facade.hpp>
#include <AngleUnit.h>
#pragma warning( push )
#pragma warning (disable:4244)
#pragma warning(disable:4715)

namespace Glas{
	template<class Container, class Value>
	class VecIterator: public boost::iterator_facade<VecIterator<Container, Value>, Value, boost::forward_traversal_tag>{
	public:
		VecIterator(Container * pBase,unsigned int index) : pBase(pBase), index(index){}
	private:
		friend class boost::iterator_core_access;
		void increment() { index++; }
		Value& dereference() const {return (*pBase)[index];}
		bool equal(const VecIterator<Container, Value>& other) const { return index == other.index; }
	private:
		unsigned int index;
		Container * pBase;
	};

	template<class T>
	class Vector2 : public irr::core::vector2d<T>{
	public:
		typedef VecIterator<Vector2<T>, T> iterator;

		Vector2(T x=0, T y=0) : irr::core::vector2d<T>(x,y){}
		Vector2(const irr::core::vector2d<T> & v){
			X = v.X;
			Y = v.Y;
		}
		Vector2<T>& operator=(const irr::core::vector2d<T> & v){
			X = v.X;
			Y = v.Y;
			return *this;
		}
		
		T& operator[](unsigned int index){
			assert(index<2);
			switch(index){
				case 0: return X;
				case 1: return Y;
			};
		}
		
		iterator begin(){return iterator(this, 0);}
	    iterator end(){return iterator(this, 2);}
	};

	typedef Vector2<float> Vector2f;


	template<class T>
	class Vector3 : public irr::core::vector3d<T>{
	public:
		typedef VecIterator<Vector3<T>, T> iterator;
		
		Vector3(T x=0, T y=0, T z=0) : irr::core::vector3d<T>(x,y,z){}
		Vector3(const irr::core::vector3d<T> & v){
			X = v.X;
			Y = v.Y;
			Z = v.Z;
		}
		Vector3<T>& operator=(const irr::core::vector3d<T> & v){
			X = v.X;
			Y = v.Y;
			Z = v.Z;
			return *this;
		}
		
		T& operator[](unsigned int index){
			assert(index<3);
			switch(index){
				case 0: return X;
				case 1: return Y;
				case 2: return Z;
			};
		}
		
		iterator begin(){return iterator(this, 0);}
	    iterator end(){return iterator(this, 3);}
	};

	typedef Vector3<float> Vector3f;
	typedef Vector3<TUL::Radian> EulerAngle3r;
	typedef Vector3<TUL::Degree> EulerAngle3d;


	class Quaternion{
	public:
		Quaternion() : q(){};
		Quaternion(irr::core::quaternion q) : q(q){};
		Quaternion(float x, float y, float z, float w) : q(x,y,z,w){}
		Quaternion(TUL::Radian x, TUL::Radian y, TUL::Radian z) : q(x.getRaw(),y.getRaw(),z.getRaw()){};
		Quaternion(const EulerAngle3r& vec) : q(vec.X.getRaw(),vec.Y.getRaw(),vec.Z.getRaw()){};

		bool operator==(const Quaternion& other) const{ return q==other.q; };

		bool operator!=(const Quaternion& other) const{ return q!=other.q; };

		inline Quaternion& operator=(const Quaternion& other){
			q = other.q; return *this;
		};

		Quaternion operator+(const Quaternion& other) const{
			Quaternion x(q+other.q); return x;
		};

		Quaternion operator*(const Quaternion& other) const{
			Quaternion x(q*other.q); return x;
		};

		Quaternion operator*(float s) const{
			Quaternion x(q*s); return x;
		};

		Vector3f operator*(const Vector3f& v) const{
			return q * v;
		}

		Quaternion& operator*=(float s){
			q *= s; return *this;
		};

		Quaternion& operator*=(const Quaternion& other){
			q *= other.q; return *this;
		};

		inline float dotProduct(const Quaternion& other) const{ return q.dotProduct(other.q); };

		inline Quaternion& set(float x, float y, float z, float w){
			q.set(x,y,z,w); return *this;
		};

		inline Quaternion& set(TUL::Radian x, TUL::Radian y, TUL::Radian z){
			q.set(x.getRaw(),y.getRaw(),z.getRaw());
			return *this;
		};

		inline Quaternion& set(const EulerAngle3r& vec){
			q.set(vec.X.getRaw(),vec.Y.getRaw(),vec.Z.getRaw());
			return *this;
		};

		inline Quaternion& set(const Quaternion& quat){
			q.set(quat.q); return *this;
		};

		inline bool equals(const Quaternion& other, const float tolerance = irr::core::ROUNDING_ERROR_f32 ) const{
			return q.equals(other.q, tolerance);
		};

		inline Quaternion& normalize(){
			q.normalize(); return *this;
		};

		Quaternion& lerp(Quaternion q1, Quaternion q2, float time){
			q.lerp(q1.q, q2.q, time); return *this;
		};

		Quaternion& slerp(Quaternion q1, Quaternion q2, float time, float threshold=.05f){
			q.slerp(q1.q, q2.q, time, threshold); return *this;
		};

		Quaternion& fromAngleAxis (TUL::Radian angle, const Vector3f& axis){
			q.fromAngleAxis(angle.getRaw(), axis); return *this;
		};

		void toAngleAxis (TUL::Radian &angle, Vector3f& axis) const{
			float x; q.toAngleAxis(x, axis);
			angle = TUL::Radian(x); 
		};

		void toEuler(EulerAngle3r& euler) const{
			irr::core::vector3df v;
			q.toEuler(v);
			euler.X = v.X;
			euler.Y = v.Y;
			euler.Z = v.Z;
		};

		Quaternion& makeIdentity(){
			q.makeIdentity(); return *this;
		};

		Quaternion& rotationFromTo(const Vector3f& from, const Vector3f& to){
			q.rotationFromTo(from, to); return *this;
		};
	private:
		irr::core::quaternion q;
	};
};
#pragma warning( pop )