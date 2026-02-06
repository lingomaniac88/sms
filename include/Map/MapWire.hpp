#ifndef MAP_MAP_WIRE_HPP
#define MAP_MAP_WIRE_HPP

#include <JSystem/JGeometry/JGVec2.hpp>
#include <JSystem/JGeometry/JGVec3.hpp>

class J3DModel;
class TCubeGeneralInfo;

class TMapWirePoint {
public:
	TMapWirePoint();

public:
	JGeometry::TVec3<f32> unk00;
	JGeometry::TVec3<f32> unk0C;
	f32 unk18;
	f32 unk1C;
	f32 unk20;
};

class TMapWire {
public:
	void drawLower() const;
	void drawUpper() const;
	f32 getPointPowerAtReleased(f32) const;
	void getPointPosAtReleased(f32, JGeometry::TVec3<f32>*) const;
	void updatePointAtReleased(int);
	void updateMovePointAtReleased();
	void initPointAtJustReleased(f32, TMapWirePoint*);
	void release();
	void getPointPosAtHanged(f32, JGeometry::TVec3<f32>*) const;
	void getPointInfoAtHanged(f32, TMapWirePoint*);
	void setFootPointsAtHanged(MtxPtr);
	void calcViewAndDBEntry();
	void move();
	f32 getPosInWire(const JGeometry::TVec3<f32>&) const;
	void getPointPosOnLine(f32, JGeometry::TVec3<f32>*) const;
	void getPointPosOnWire(f32, JGeometry::TVec3<f32>*) const;
	void getPointPosDefault(f32, JGeometry::TVec3<f32>*) const;
	void initTipPoints(const TCubeGeneralInfo*);
	void init(const TCubeGeneralInfo*);
	TMapWire();

	static f32 mMoveTimerSpeed;
	static f32 mDownRateMax;
	static f32 mEndRate;
	static f32 mStretchRate;
	static f32 mHeightRate;
	static f32 mReleaseHeight;
	static f32 mFootLength;
	static f32 mDrawWidth;
	static f32 mDrawHeight;

	// fabricated
	const JGeometry::TVec3<f32>& getStartPoint() const { return mStartPoint; }
	const JGeometry::TVec3<f32>& getEndPoint() const { return mEndPoint; }

	// very fake
	void getPointPosAtHangedInlined(f32, JGeometry::TVec3<f32>*) const;

public:
	/* 0x00 */ JGeometry::TVec3<f32> mStartPoint;
	/* 0x0C */ JGeometry::TVec3<f32> mEndPoint;
	/* 0x18 */ JGeometry::TVec3<f32> mDir;
	/* 0x24 */ f32 unk24;
	/* 0x28 */ f32 unk28;
	/* 0x2C */ f32 unk2C;
	/* 0x30 */ f32 unk30;
	/* 0x34 */ f32 unk34;
	/* 0x38 */ f32 unk38;
	/* 0x3C */ J3DModel* unk3C;
	/* 0x40 */ J3DModel* unk40;
	/* 0x44 */ u16 mNumActiveMapWirePoints;
	/* 0x46 */ u16 mNumMapWirePoints;
	/* 0x48 */ TMapWirePoint* mMapWirePoints;
	/* 0x4C */ f32 unk4C;
	/* 0x50 */ JGeometry::TVec3<f32> unk50;
	/* 0x5C */ f32 unk5C;
	/* 0x60 */ f32 unk60;
	/* 0x64 */ f32 unk64;
	/* 0x68 */ f32 unk68;
	/* 0x6C */ JGeometry::TVec2<f32> unk6C;
	/* 0x74 */ f32 unk74;
	/* 0x78 */ f32 unk78;
	/* 0x7C */ u16 unk7C; // Probably an enum?
};

#endif
