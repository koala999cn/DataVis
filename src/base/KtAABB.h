#pragma once
#include "KtPoint.h"
#include <assert.h>


/** A axes aligned bounding box.
@remarks
This class represents a simple 2d/3d box which is aligned with the
axes. Internally it only stores 2 points as the extremeties of
the box, one which is the minima of all 2/3 axes, and the other
which is the maxima of all 2/3 axes. 
*/
template<typename T, int DIM = 3>
class KtAABB
{
	using point = KtPoint<T, DIM>;
	using kMath = KtuMath<T>;

public:

	/*
		1-----2
	   /|    /|
	  / |   / |
	 5-----4  |
	 |  0--|--3
	 | /   | /
	 |/    |/
	 6-----7
	*/
	enum KeCorner
	{
		k_far_left_bottom,
		k_far_left_top,
		k_far_right_top,
		k_far_right_bottom,
		k_near_right_top,
		k_near_left_top,
		k_near_left_bottom,
		k_near_right_bottom
	};

	// Default to a null box 
	KtAABB() { setNull(); }

	KtAABB(const KtAABB& box)
		: lower_(box.lower_), upper_(box.upper_) {}

	KtAABB(const point& lower, const point& upper) {
		lower_ = lower_.floor(lower, upper);
		upper_ = upper_.ceil(lower, upper);
	}


	KtAABB& operator=(const KtAABB& rhs) {
		// Specifically override to avoid copying m_pkCorners
		lower_ = rhs.lower_;
		upper_ = rhs.upper_;
		return *this;
	}


	/** Gets the minimum corner of the box.
	*/
	const point& lower() const { return lower_; }

	/** Gets a modifiable version of the minimum
	corner of the box.
	*/
	point& lower() { return lower_; }


	/** Gets the maximum corner of the box.
	*/
	const point& upper() const { return upper_; }

	/** Gets a modifiable version of the maximum
	corner of the box.
	*/
	point& upper() { return upper_; }


	/** Sets both minimum and maximum extents at once.
	*/
	void setExtents(const point& lower, const point& upper) {
		assert(lower.leAll(upper));
		lower_ = lower, upper_ = upper;
	}


	/** gets the position of one of the corners
	*/
	point corner(KeCorner id) const;

	/** Returns an array of 4/8 corner points, useful for
	collision vs. non-aligned objects.
	*/
	std::vector<point> allCorners() const {
		std::vector<point> pts(4 * (DIM - 1));
		for (int i = 0; i < pts.size(); i++)
			pts[i] = corner(KeBoxCorner(i));
		return pts;
	}


	/// Gets the centre of the box
	point center() const {
		return isNull() ? point(0) : (lower_ + upper_) / 2;
	}

	/// Gets the size of the box
	point size() const {
		return isNull() ? point(0) : upper_ - lower_;
	}

	/// Calculate the volume of this box
	T volume() const {
		auto sz = size();
		return kMath::product(sz.data(), sz.size());
	}

	T width() const { return upper_.x() - lower_.x(); }

	T height() const { return upper_.y() - lower_.y(); }

	T depth() const { return upper_.z() - lower_.z(); }

	/** Sets the box to a 'null' value i.e. not a box.
	*/
	void setNull() {
		lower_ = upper_ = point(kMath::nan);
	}

	/** Returns true if the box is null i.e. empty.
	*/
	bool isNull() const {
		return lower_.isNan() || upper_.isNan();
	}

	/** Sets the box to 'infinite'
	*/
	void setInf() {
		setExtents(point(-kMath::inf), point(kMath::inf));
	}

	/** Returns true if the box is infinite.
	*/
	bool isInf() const {
		return lower_.isInf() || upper_.isInf();
	}

	/** Returns true if the box is finite.
	*/
	bool isFinite() const {
		return !isNull() && !isInf();
	}

	/** Tests 2 boxes for equality.
	*/
	bool operator ==(const KtAABB& rhs) const {
		return lower_ == rhs.lower_ && upper_ == rhs.upper_;
	}

	/** Tests 2 boxes for inequality.
	*/
	bool operator !=(const KtAABB& rhs) const {
		return lower_ != rhs.lower_ || upper_ != rhs.upper_;
	}

	/** Tests whether the given point contained by this box.
	*/
	bool contains(const point& v) const;

	/** Tests whether another box contained by this box.
	*/
	bool contains(const KtAABB& rhs) const;

	/** Scales the AABB by the vector given. */
	KtAABB& scale(const point& v) {
		// NB assumes centered on origin
		lower_ *= v, upper_ *= v;
		return *this;
	}

	KtAABB& shrink(const point& lw, const point& up) {
		lower_ += lw, upper_ -= up;
		return *this;
	}

	KtAABB& expand(const point& lw, const point& up) {
		lower_ -= lw, upper_ += up;
		return *this;
	}

	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	KtAABB& merge(const KtAABB& rhs);

	/** Extends the box to encompass the specified point (if needed).
	*/
	KtAABB& merge(const point& pt);

	/// Calculate the area of intersection of this box and another
	KtAABB intersection(const KtAABB& rhs) const;

	/** Returns whether or not this box intersects another. */
	bool isIntersects(const KtAABB& rhs) const;

private:
	KtPoint<T, DIM> lower_, upper_;
};

template<typename T, int DIM>
KtPoint<T, DIM> KtAABB<T, DIM>::corner(KeCorner id) const
{
	if (isNull())
		return point(0);

	switch (id)
	{
	case k_far_left_bottom:
		return lower_;

	case k_far_left_top:
		return DIM == 3 ? { lower_.x(), upper_.y(), lower_.z() } : { lower_.x(), upper_.y() };

	case k_far_right_top:
		return DIM == 3 ? { upper_.x(), upper_.y(), lower_.z() } : { upper_.x(), upper_.y() };

	case k_far_right_bottom:
		return DIM == 3 ? { upper_.x(), lower_.y(), lower_.z() } : { upper_.x(), lower_.y() };

	case k_near_right_bottom:
		return DIM == 3 ? { upper_.x(), lower_.y(), upper_.z() } : { upper_.x(), lower_.y() };

	case k_near_left_bottom:
		return DIM == 3 ? { lower_.x(), lower_.y(), upper_.z() } : { lower_.x(), lower_.y() };

	case k_near_left_top:
		return DIM == 3 ? { lower_.x(), upper_.y(), upper_.z() } : { lower_.x(), upper_.y() };

	case k_near_right_top:
		return upper_;

	default:
		assert(false);
	}

	return { 0 }; // make compiler happy
}


template<typename T, int DIM>
bool KtAABB<T, DIM>::contains(const point& v) const
{
	if (isNull())
		return false;
	else if (isInf())
		return true;

	return v.geAll(lower) && v.leAll(upper);
}

template<typename T, int DIM>
bool KtAABB<T, DIM>::contains(const KtAABB& rhs) const
{
	if (isInf() || rhs.isNull())
		return true;

	if (isNull())
		return false;

	return lower_.leAll(rhs.lower()) && upper_.geAll(rhs.upper()); 
}

template<typename T, int DIM>
KtAABB<T, DIM>& KtAABB<T, DIM>::merge(const point& pt)
{
	if (isNull()) // if null, use this point
		setExtents(pt, pt);
	else if (isFinite()) {
		auto lower = point::floor(lower_, pt);
		auto upper = point::ceil(upper_, pt);
		setExtents(lower, upper);
	}

	return *this;
}

template<typename T, int DIM>
KtAABB<T, DIM>& KtAABB<T, DIM>::merge(const KtAABB& rhs)
{
	// Do nothing if rhs null, or this is infinite
	if (rhs.isNull() || isInf())
		return *this;
	// Otherwise if rhs is infinite, make this infinite, too
	else if (rhs.isInf())
		setInf();
	// Otherwise if current null, just take rhs
	else if (isNull())
		*this = rhs;
	// Otherwise merge
	else {
		auto lower = point::floor(lower_, rhs.lower_);
		auto upper = point::ceil(upper_, rhs.upper_);
		setExtents(lower, upper);
	}

	return *this;
}

template<typename T, int DIM>
KtAABB<T, DIM> KtAABB<T, DIM>::intersection(const KtAABB& rhs) const
{
	if (isNull() || rhs.isNull())
		return KtAABB();
	else if (isInf())
		return rhs;
	else if (rhs.isInf())
		return *this;

	auto lower = point::ceil(lower_, rhs.lower_);
	auto upper = point::floor(upper_, rhs.upper_);

	// Check intersection isn't null
	if (lower.leAll(upper))
		return KtAABB(lower, upper);

	return KtAABB();
}

template<typename T, int DIM>
bool KtAABB<T, DIM>::isIntersects(const KtAABB& rhs) const
{
	return !intersection(rhs).isNull();
}