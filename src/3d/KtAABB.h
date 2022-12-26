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
	using kMath = KtuMath<T>;

public:

	using point_t = KtPoint<T, DIM>;

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

	KtAABB(const point_t& lower, const point_t& upper) {
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
	const point_t& lower() const { return lower_; }

	/** Gets a modifiable version of the minimum
	corner of the box.
	*/
	point_t& lower() { return lower_; }


	/** Gets the maximum corner of the box.
	*/
	const point_t& upper() const { return upper_; }

	/** Gets a modifiable version of the maximum
	corner of the box.
	*/
	point_t& upper() { return upper_; }


	/** Sets both minimum and maximum extents at once.
	*/
	void setExtents(const point_t& lower, const point_t& upper) {
		assert(lower.le(upper));
		lower_ = lower, upper_ = upper;
	}

	void setExtent(int dim, T ext) { upper_[dim] = lower_[dim] + ext; }

	T extent(int dim) const { return upper_[dim] - lower_[dim]; }

	T width() const { return extent(0); }

	T height() const { return extent(1); }

	T depth() const { return extent(2); }

	/** gets the position of one of the corners
	*/
	point_t corner(KeCorner id) const;

	/** Returns an array of 4/8 corner points, useful for
	collision vs. non-aligned objects.
	*/
	std::vector<point_t> allCorners() const {
		std::vector<point_t> pts(4 * (DIM - 1));
		for (int i = 0; i < pts.size(); i++)
			pts[i] = corner(KeCorner(i));
		return pts;
	}


	/// Gets the centre of the box
	point_t center() const {
		return isNull() ? point_t(0) : (lower_ + upper_) / 2;
	}

	/// Gets the size of the box
	point_t size() const {
		return isNull() ? point_t(0) : upper_ - lower_;
	}

	/// Calculate the volume of this box
	T volume() const {
		auto sz = size();
		return kMath::product(sz.data(), sz.size());
	}

	/** Sets the box to a 'null' value i.e. not a box.
	*/
	void setNull() {
		lower_ = upper_ = point_t(kMath::nan);
	}

	/** Returns true if the box is null i.e. empty.
	*/
	bool isNull() const {
		return lower_.isNan() || upper_.isNan();
	}

	/** Sets the box to 'infinite'
	*/
	void setInf() {
		setExtents(point_t(-kMath::inf), point_t(kMath::inf));
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
	bool contains(const point_t& v) const;

	/** Tests whether another box contained by this box.
	*/
	bool contains(const KtAABB& rhs) const;

	/** Scales the AABB by the vector given. */
	KtAABB& scale(const point_t& v) {
		// NB assumes centered on origin
		lower_ *= v, upper_ *= v;
		return *this;
	}

	KtAABB& scale(T v) {
		// NB assumes centered on origin
		lower_ *= v, upper_ *= v;
		return *this;
	}

	KtAABB& deflate(const point_t& lw, const point_t& up) {
		lower_ += lw, upper_ -= up;
		return *this;
	}

	KtAABB& deflate(const KtAABB& d) {
		deflate(d.lower(), d.upper());
		return *this;
	}

	KtAABB& deflate(float_t dx, float_t dy) {
		point_t d(dx, dy);
		return deflate(d, d);
	}

	KtAABB& deflate(float_t d) {
		return deflate(d, d);
	}

	KtAABB& inflate(const point_t& lw, const point_t& up) {
		lower_ -= lw, upper_ += up;
		return *this;
	}

	KtAABB& inflate(const KtAABB& d) {
		expand(d.lower(), d.upper());
		return *this;
	}

	KtAABB& inflate(float_t dx, float_t dy) {
		point_t d(dx, dy);
		return inflate(d, d);
	}

	KtAABB& inflate(float_t d) {
		return inflate(d, d);
	}

	KtAABB& shift(const point_t& d) {
		lower_ += d, upper_ += d;
		return *this;
	}

	/** Merges the passed in box into the current box. The result is the
	box which encompasses both.
	*/
	KtAABB& merge(const KtAABB& rhs);

	/** Extends the box to encompass the specified point (if needed).
	*/
	KtAABB& merge(const point_t& pt);

	/// Calculate the area of intersection of this box and another
	KtAABB intersection(const KtAABB& rhs) const;

	/** Returns whether or not this box intersects another. */
	bool isIntersects(const KtAABB& rhs) const;

private:
	point_t lower_, upper_;
};

template<typename T, int DIM>
typename KtAABB<T, DIM>::point_t KtAABB<T, DIM>::corner(KeCorner id) const
{
	if (isNull())
		return point_t(0);

	switch (id)
	{
	case k_far_left_bottom:
		return lower_;

	case k_far_left_top:
		if constexpr (DIM == 3)
			return { lower_.x(), upper_.y(), lower_.z() };
		else 
		    return{ lower_.x(), upper_.y() };

	case k_far_right_top:
		if constexpr (DIM == 3)
			return { upper_.x(), upper_.y(), lower_.z() };
		else 
		    return{ upper_.x(), upper_.y() };

	case k_far_right_bottom:
		if constexpr (DIM == 3)
			return { upper_.x(), lower_.y(), lower_.z() };
		else
			return { upper_.x(), lower_.y() };

	case k_near_right_bottom:
		if constexpr (DIM == 3)
			return { upper_.x(), lower_.y(), upper_.z() };
		else
			return { upper_.x(), lower_.y() };

	case k_near_left_bottom:
		if constexpr (DIM == 3)
			return { lower_.x(), lower_.y(), upper_.z() };
		else
			return { lower_.x(), lower_.y() };

	case k_near_left_top:
		if constexpr (DIM == 3)
			return { lower_.x(), upper_.y(), upper_.z() };
		else
			return { lower_.x(), upper_.y() };

	case k_near_right_top:
		return upper_;

	default:
		assert(false);
	}

	return point_t(0); // make compiler happy
}


template<typename T, int DIM>
bool KtAABB<T, DIM>::contains(const point_t& v) const
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
KtAABB<T, DIM>& KtAABB<T, DIM>::merge(const point_t& pt)
{
	if (isNull()) // if null, use this point
		setExtents(pt, pt);
	else if (isFinite()) {
		auto lower = point_t::floor(lower_, pt);
		auto upper = point_t::ceil(upper_, pt);
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
		auto lower = point_t::floor(lower_, rhs.lower_);
		auto upper = point_t::ceil(upper_, rhs.upper_);
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

	auto lower = point_t::ceil(lower_, rhs.lower_);
	auto upper = point_t::floor(upper_, rhs.upper_);

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