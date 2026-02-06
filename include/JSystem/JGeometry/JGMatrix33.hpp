#ifndef JG_MATRIX33_HPP
#define JG_MATRIX33_HPP

#include <dolphin/types.h>
#include <JSystem/JGeometry/JGVec3.hpp>

namespace JGeometry {

template <typename T> struct SMatrix33C {
	T mMtx[3][3];
};

// Same explicit specialization trick as with 3x4 and 4x4 versions
template <> struct SMatrix33C<f32> {
	f32 at(u32 i, u32 j) const { return mMtx[i][j]; }
	f32& ref(u32 i, u32 j) { return mMtx[i][j]; }

	f32 mMtx[3][3];
};

template <typename T> struct TMatrix33 : public T {
	TMatrix33() { }

	// fabricated, possibly fake but still useful
	static inline void multiplyInPlace(const TMatrix33& mtx, TVec3<f32>* vec)
	{
		vec->set(mtx.at(0, 0) * vec->x + mtx.at(0, 1) * vec->y
		             + mtx.at(0, 2) * vec->z,
		         mtx.at(1, 0) * vec->x + mtx.at(1, 1) * vec->y
		             + mtx.at(1, 2) * vec->z,
		         mtx.at(2, 0) * vec->x + mtx.at(2, 1) * vec->y
		             + mtx.at(2, 2) * vec->z);
	}

	// fabricated and very fake
	static inline void multiplyInPlaceNest1(const TMatrix33& mtx,
	                                        TVec3<f32>* vec)
	{
		multiplyInPlace(mtx, vec);
	}

	// fabricated and very fake
	static inline void multiplyInPlaceNest2(const TMatrix33& mtx,
	                                        TVec3<f32>* vec)
	{
		multiplyInPlaceNest1(mtx, vec);
	}
};

} // namespace JGeometry

#endif
