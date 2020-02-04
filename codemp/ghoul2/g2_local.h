/*
===========================================================================
Copyright (C) 2000 - 2013, Raven Software, Inc.
Copyright (C) 2001 - 2013, Activision, Inc.
Copyright (C) 2013 - 2019, OpenJK contributors
Copyright (C) 2019 - 2020, CleanJoKe contributors

This file is part of the OpenJK source code.

OpenJK is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#pragma once

#include "ghoul2/ghoul2_shared.h"

#define GHOUL2_CRAZY_SMOOTH		0x2000		//hack for smoothing during ugly situations. forgive me.

class IHeapAllocator;
class CRagDollUpdateParams;
class CRagDollParams;

class IGhoul2InfoArray
{
public:
	virtual ~IGhoul2InfoArray() {}

	virtual int New()=0;
	virtual void Delete(int handle)=0;
	virtual bool IsValid(int handle) const=0;
	virtual std::vector<CGhoul2Info> &Get(int handle)=0;
	virtual const std::vector<CGhoul2Info> &Get(int handle) const=0;
};

IGhoul2InfoArray &TheGhoul2InfoArray();
class CGhoul2Info_v
{
	IGhoul2InfoArray &InfoArray() const
	{
		return TheGhoul2InfoArray();
	}

	void Alloc()
	{
		assert(!mItem); //already alloced
		mItem=InfoArray().New();
		assert(!Array().size());
	}
	void Free()
	{
		if (mItem)
		{
			assert(InfoArray().IsValid(mItem));
			InfoArray().Delete(mItem);
			mItem=0;
		}
	}
	std::vector<CGhoul2Info> &Array()
	{
		assert(InfoArray().IsValid(mItem));
		return InfoArray().Get(mItem);
	}
	const std::vector<CGhoul2Info> &Array() const
	{
		assert(InfoArray().IsValid(mItem));
		return InfoArray().Get(mItem);
	}
public:
	int mItem;	//dont' be bad and muck with this
	CGhoul2Info_v()
	{
		mItem=0;
	}
	CGhoul2Info_v(const int item)
	{	//be VERY carefull with what you pass in here
		mItem=item;
	}
	~CGhoul2Info_v()
	{
		Free(); //this had better be taken care of via the clean ghoul2 models call
	}
	void operator=(const CGhoul2Info_v &other)
	{
		mItem=other.mItem;
	}
	void operator=(const int otherItem)	//assigning one from the VM side item number
	{
		mItem=otherItem;
	}
	void DeepCopy(const CGhoul2Info_v &other)
	{
		Free();
		if (other.mItem)
		{
			Alloc();
			Array()=other.Array();
			int i;
			for (i=0;i<size();i++)
			{
				Array()[i].mBoneCache=0;
				Array()[i].mTransformedVertsArray=0;
				Array()[i].mSkelFrameNum=0;
				Array()[i].mMeshFrameNum=0;
			}
		}
	}
	CGhoul2Info &operator[](int idx)
	{
		assert (mItem);
		assert (idx >= 0 && idx < size());

		return Array()[idx];
	}
	const CGhoul2Info &operator[](int idx) const
	{
		assert (mItem);
		assert (idx >= 0 && idx < size());

		return Array()[idx];
	}
	void resize(int num)
	{
		assert(num>=0);
		if (num)
		{
			if (!mItem)
			{
				Alloc();
			}
		}
		if (mItem||num)
		{
			Array().resize(num);
		}
	}
	void clear()
	{
		Free();
	}
	void push_back(const CGhoul2Info &model)
	{
		if (!mItem)
		{
			Alloc();
		}
		Array().push_back(model);
	}
	int size() const
	{
		if (!IsValid())
		{
			return 0;
		}
		return Array().size();
	}
	bool IsValid() const
	{
		return InfoArray().IsValid(mItem);
	}
	void kill()
	{
		// this scary method zeros the infovector handle without actually freeing it
		// it is used for some places where a copy is made, but we don't want to go through the trouble
		// of making a deep copy
		mItem=0;
	}
};

void Create_Matrix(const float *angle, mdxaBone_t *matrix);

extern mdxaBone_t	worldMatrix;
extern mdxaBone_t	worldMatrixInv;
extern bool			gG2_GBMNoReconstruct;
extern bool			gG2_GBMUseSPMethod;

// internal surface calls  G2_surfaces.cpp
bool G2_RemoveSurface(surfaceInfo_v& slist, const int index);
bool G2_SetRootSurface(CGhoul2Info_v& ghoul2, const int modelIndex, const char* surfaceName);
bool G2_SetSurfaceOnOff(CGhoul2Info* ghlInfo, surfaceInfo_v& slist, const char* surfaceName, const int offFlags);
int			G2_AddSurface(CGhoul2Info *ghoul2, int surfaceNumber, int polyNumber, float BarycentricI, float BarycentricJ, int lod );
int			G2_GetParentSurface(CGhoul2Info *ghlInfo, const int index);
int			G2_GetSurfaceIndex(CGhoul2Info *ghlInfo, const char *surfaceName);
int	G2_IsSurfaceLegal(void* mod, const char* surfaceName, int* flags);
int	G2_IsSurfaceOff(CGhoul2Info* ghlInfo, surfaceInfo_v& slist, const char* surfaceName);
int			G2_IsSurfaceRendered(CGhoul2Info *ghlInfo, const char *surfaceName, surfaceInfo_v &slist);
surfaceInfo_t* G2_FindOverrideSurface(int surfaceNum, surfaceInfo_v& surfaceList);

// internal bone calls - G2_Bones.cpp
bool G2_Get_Bone_Anim(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName, const int currentTime, float* currentFrame, int* startFrame, int* endFrame, int* flags, float* retAnimSpeed, qhandle_t* modelList, int modelIndex);
bool G2_Get_Bone_Anim_Index(boneInfo_v& blist, const int index, const int currentTime, float* currentFrame, int* startFrame, int* endFrame, int* flags, float* retAnimSpeed, qhandle_t* modelList, int modelIndex);
bool G2_Get_Bone_Anim_Range(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName, int* startFrame, int* endFrame);
bool G2_IsPaused(const char* fileName, boneInfo_v& blist, const char* boneName);
bool G2_Pause_Bone_Anim(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName, const int currentTime);
bool G2_Remove_Bone(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName);
bool G2_Set_Bone_Angles(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName, const float* angles, const int flags, const Eorientations_e up, const Eorientations_e left, const Eorientations_e forward, qhandle_t* modelList, const int modelIndex, const int blendTime, const int currentTime);
bool G2_Set_Bone_Angles_Index(boneInfo_v& blist, const int index, const float* angles, const int flags, const Eorientations_e yaw, const Eorientations_e pitch, const Eorientations_e roll, qhandle_t* modelList, const int modelIndex, const int blendTime, const int currentTime);
bool G2_Set_Bone_Angles_Matrix(const char* fileName, boneInfo_v& blist, const char* boneName, const mdxaBone_t& matrix, const int flags, qhandle_t* modelList, const int modelIndex, const int blendTime, const int currentTime);
bool G2_Set_Bone_Angles_Matrix_Index(boneInfo_v& blist, const int index, const mdxaBone_t& matrix, const int flags, qhandle_t* modelList, const int modelIndex, const int blendTime, const int currentTime);
bool G2_Set_Bone_Anim(CGhoul2Info* ghlInfo, boneInfo_v& blist, const char* boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime);
bool G2_Set_Bone_Anim_Index(boneInfo_v& blist, const int index, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime, const int numFrames);
bool G2_Stop_Bone_Angles(const char* fileName, boneInfo_v& blist, const char* boneName);
bool G2_Stop_Bone_Angles_Index(boneInfo_v& blist, const int index);
bool G2_Stop_Bone_Anim(const char* fileName, boneInfo_v& blist, const char* boneName);
bool G2_Stop_Bone_Anim_Index(boneInfo_v& blist, const int index);
int	G2_Find_Bone_In_List(boneInfo_v& blist, const int boneNum);
int	G2_Get_Bone_Index(CGhoul2Info* ghoul2, const char* boneName);
void G2_Init_Bone_List(boneInfo_v& blist, int numBones);
void G2_RemoveRedundantBoneOverrides(boneInfo_v& blist, int* activeBones);
//rww - RAGDOLL_BEGIN
void		G2_Animate_Bone_List(CGhoul2Info_v &ghoul2, const int currentTime, const int index,CRagDollUpdateParams *params);
//rww - RAGDOLL_END

// misc functions G2_misc.cpp
bool G2_GetAnimFileName(const char* fileName, char** filename);
bool G2_SaveGhoul2Models(CGhoul2Info_v& ghoul2, char** buffer, int* size);
void G2_GenerateWorldMatrix(const vec3_t angles, const vec3_t origin);
void G2_List_Model_Bones(const char* fileName, int frame);
void		G2_List_Model_Surfaces(const char *fileName);
void G2_LoadGhoul2Model(CGhoul2Info_v& ghoul2, char* buffer);
void Inverse_Matrix(mdxaBone_t* src, mdxaBone_t* dest);
void TransformAndTranslatePoint(const vec3_t in, vec3_t out, mdxaBone_t* mat);
void TransformPoint(const vec3_t in, vec3_t out, mdxaBone_t* mat);
void* G2_FindSurface(void* mod, int index, int lod);

#ifdef _G2_GORE
void		G2_TraceModels(CGhoul2Info_v& ghoul2, vec3_t rayStart, vec3_t rayEnd, CollisionRecord_t* collRecMap, int entNum, int traceFlags, int useLod, float fRadius, float ssize, float tsize, float theta, int shader, SSkinGoreData* gore, bool skipIfLODNotMatch);
#else
void		G2_TraceModels(CGhoul2Info_v &ghoul2, vec3_t rayStart, vec3_t rayEnd, CollisionRecord_t *collRecMap, int entNum, int traceFlags, int useLod, float fRadius);
#endif

#ifdef _G2_GORE
void		G2_TransformModel(CGhoul2Info_v &ghoul2, const int frameNum, vec3_t scale, IHeapAllocator *G2VertSpace, int useLod, bool ApplyGore);
#else
void		G2_TransformModel(CGhoul2Info_v &ghoul2, const int frameNum, vec3_t scale, IHeapAllocator *G2VertSpace, int useLod);
#endif

// internal bolt calls. G2_bolts.cpp
bool G2_Remove_Bolt(boltInfo_v& bltlist, int index);
int			G2_Add_Bolt(CGhoul2Info *ghlInfo, boltInfo_v &bltlist, surfaceInfo_v &slist, const char *boneName);
int	G2_Add_Bolt_Surf_Num(CGhoul2Info* ghlInfo, boltInfo_v& bltlist, surfaceInfo_v& slist, const int surfNum);
int			G2_Find_Bolt_Bone_Num(boltInfo_v &bltlist, const int boneNum);
int			G2_Find_Bolt_Surface_Num(boltInfo_v &bltlist, const int surfaceNum, const int flags);
void G2_Init_Bolt_List(boltInfo_v& bltlist);
void		G2_RemoveRedundantBolts(boltInfo_v &bltlist, surfaceInfo_v &slist, int *activeSurfaces, int *activeBones);

// API calls - G2_API.cpp
bool G2API_SetBoneAngles(CGhoul2Info_v& ghoul2, const int modelIndex, const char* boneName, const vec3_t angles, const int flags, const Eorientations_e up, const Eorientations_e left, const Eorientations_e forward, qhandle_t* modelList, int blendTime, int currentTime);
bool G2API_AttachEnt(int* boltInfo, CGhoul2Info_v& ghoul2, int modelIndex, int toBoltIndex, int entNum, int toModelNum);
bool G2API_AttachG2Model(CGhoul2Info_v& ghoul2From, int modelFrom, CGhoul2Info_v& ghoul2To, int toBoltIndex, int toModel);
bool G2API_DetachG2Model(CGhoul2Info* ghlInfo);
bool G2API_DoesBoneExist(CGhoul2Info_v& ghoul2, int modelIndex, const char* boneName);
bool G2API_GetAnimFileName(CGhoul2Info* ghlInfo, char** filename);
bool G2API_GetAnimRange(CGhoul2Info* ghlInfo, const char* boneName, int* startFrame, int* endFrame);
bool G2API_GetBoltMatrix(CGhoul2Info_v& ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t* matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t* modelList, vec3_t scale);
bool G2API_GetBoneAnim(CGhoul2Info_v& ghoul2, int modelIndex, const char* boneName, const int currentTime, float* currentFrame, int* startFrame, int* endFrame, int* flags, float* animSpeed, qhandle_t* modelList);
bool G2API_GetRagBonePos(CGhoul2Info_v& ghoul2, const char* boneName, vec3_t pos, vec3_t entAngles, vec3_t entPos, vec3_t entScale);
bool G2API_HasGhoul2ModelOnIndex(CGhoul2Info_v** ghlRemove, const int modelIndex);
bool G2API_HaveWeGhoul2Models(CGhoul2Info_v& ghoul2);
bool G2API_IKMove(CGhoul2Info_v& ghoul2, int time, sharedIKMoveParams_t* params);
bool G2API_IsGhoul2InfovValid(CGhoul2Info_v& ghoul2);
bool G2API_IsPaused(CGhoul2Info* ghlInfo, const char* boneName);
bool G2API_OverrideServerWithClientData(CGhoul2Info_v& ghoul2, int modelIndex);
bool G2API_PauseBoneAnim(CGhoul2Info* ghlInfo, const char* boneName, const int currentTime);
bool G2API_RagEffectorGoal(CGhoul2Info_v& ghoul2, const char* boneName, vec3_t pos);
bool G2API_RagEffectorKick(CGhoul2Info_v& ghoul2, const char* boneName, vec3_t velocity);
bool G2API_RagForceSolve(CGhoul2Info_v& ghoul2, bool force);
bool G2API_RagPCJConstraint(CGhoul2Info_v& ghoul2, const char* boneName, vec3_t min, vec3_t max);
bool G2API_RagPCJGradientSpeed(CGhoul2Info_v& ghoul2, const char* boneName, const float speed);
bool G2API_RemoveBolt(CGhoul2Info* ghlInfo, const int index);
bool G2API_RemoveBone(CGhoul2Info_v& ghoul2, int modelIndex, const char* boneName);
bool G2API_RemoveGhoul2Model(CGhoul2Info_v** ghlRemove, const int modelIndex);
bool G2API_RemoveGhoul2Models(CGhoul2Info_v** ghlRemove);
bool G2API_RemoveSurface(CGhoul2Info* ghlInfo, const int index);
bool G2API_SaveGhoul2Models(CGhoul2Info_v& ghoul2, char** buffer, int* size);
bool G2API_SetBoneAnglesIndex(CGhoul2Info* ghlInfo, const int index, const vec3_t angles, const int flags, const Eorientations_e yaw, const Eorientations_e pitch, const Eorientations_e roll, qhandle_t* modelList, int blendTime, int currentTime);
bool G2API_SetBoneAnglesMatrix(CGhoul2Info* ghlInfo, const char* boneName, const mdxaBone_t& matrix, const int flags, qhandle_t* modelList, int blendTime = 0, int currentTime = 0);
bool G2API_SetBoneAnglesMatrixIndex(CGhoul2Info* ghlInfo, const int index, const mdxaBone_t& matrix, const int flags, qhandle_t* modelList, int blendTime, int currentTime);
bool G2API_SetBoneAnim(CGhoul2Info_v& ghoul2, const int modelIndex, const char* boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame = -1, const int blendTime = -1);
bool G2API_SetBoneAnimIndex(CGhoul2Info* ghlInfo, const int index, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime);
bool G2API_SetBoneIKState(CGhoul2Info_v& ghoul2, int time, const char* boneName, int ikState, sharedSetBoneIKStateParams_t* params);
bool G2API_SetGhoul2ModelFlags(CGhoul2Info* ghlInfo, const int flags);
bool G2API_SetLodBias(CGhoul2Info* ghlInfo, int lodBias);
bool G2API_SetNewOrigin(CGhoul2Info_v& ghoul2, const int boltIndex);
bool G2API_SetRootSurface(CGhoul2Info_v& ghoul2, const int modelIndex, const char* surfaceName);
bool G2API_SetShader(CGhoul2Info* ghlInfo, qhandle_t customShader);
bool G2API_SetSkin(CGhoul2Info_v& ghoul2, int modelIndex, qhandle_t customSkin, qhandle_t renderSkin);
bool G2API_SetSurfaceOnOff(CGhoul2Info_v& ghoul2, const char* surfaceName, const int flags);
bool G2API_StopBoneAngles(CGhoul2Info* ghlInfo, const char* boneName);
bool G2API_StopBoneAnglesIndex(CGhoul2Info* ghlInfo, const int index);
bool G2API_StopBoneAnim(CGhoul2Info* ghlInfo, const char* boneName);
bool G2API_StopBoneAnimIndex(CGhoul2Info* ghlInfo, const int index);
char* G2API_GetAnimFileNameIndex(qhandle_t modelIndex);
char* G2API_GetGLAName(CGhoul2Info_v& ghoul2, int modelIndex);
char* G2API_GetSurfaceName(CGhoul2Info_v& ghoul2, int modelIndex, int surfNumber);
int	G2API_AddBolt(CGhoul2Info_v& ghoul2, const int modelIndex, const char* boneName);
int	G2API_AddBoltSurfNum(CGhoul2Info* ghlInfo, const int surfIndex);
int	G2API_AddSurface(CGhoul2Info* ghlInfo, int surfaceNumber, int polyNumber, float BarycentricI, float BarycentricJ, int lod);
int	G2API_CopyGhoul2Instance(CGhoul2Info_v& g2From, CGhoul2Info_v& g2To, int modelIndex);
int	G2API_GetBoneIndex(CGhoul2Info* ghlInfo, const char* boneName);
int	G2API_GetGhoul2ModelFlags(CGhoul2Info* ghlInfo);
int	G2API_GetParentSurface(CGhoul2Info* ghlInfo, const int index);
int	G2API_GetSurfaceIndex(CGhoul2Info* ghlInfo, const char* surfaceName);
int	G2API_GetSurfaceOnOff(CGhoul2Info* ghlInfo, const char* surfaceName);
int	G2API_GetSurfaceRenderStatus(CGhoul2Info_v& ghoul2, int modelIndex, const char* surfaceName);
int			G2API_GetTime(int argTime);
int	G2API_InitGhoul2Model(CGhoul2Info_v** ghoul2Ptr, const char* fileName, int modelIndex, qhandle_t customSkin = NULL_HANDLE, qhandle_t customShader = NULL_HANDLE, int modelFlags = 0, int lodBias = 0);
qhandle_t	G2API_PrecacheGhoul2Model(const char *fileName);
void G2API_AbsurdSmoothing(CGhoul2Info_v& ghoul2, bool status);
void		G2API_AnimateG2Models(CGhoul2Info_v &ghoul2, float speedVar);
void G2API_AnimateG2ModelsRag(CGhoul2Info_v& ghoul2, int AcurrentTime, CRagDollUpdateParams* params);
void G2API_AttachInstanceToEntNum(CGhoul2Info_v& ghoul2, int entityNum, bool server);
void G2API_CleanEntAttachments(void);
void G2API_CleanGhoul2Models(CGhoul2Info_v** ghoul2Ptr);
void G2API_ClearAttachedInstance(int entityNum);
void		G2API_CollisionDetect(CollisionRecord_t *collRecMap, CGhoul2Info_v &ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, IHeapAllocator *G2VertSpace, int traceFlags, int useLod, float fRadius);
void		G2API_CollisionDetectCache(CollisionRecord_t *collRecMap, CGhoul2Info_v &ghoul2, const vec3_t angles, const vec3_t position, int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, IHeapAllocator *G2VertSpace, int traceFlags, int useLod, float fRadius);
void G2API_CopySpecificG2Model(CGhoul2Info_v& ghoul2From, int modelFrom, CGhoul2Info_v& ghoul2To, int modelTo);
void G2API_DetachEnt(int* boltInfo);
void G2API_DuplicateGhoul2Instance(CGhoul2Info_v& g2From, CGhoul2Info_v** g2To);
void G2API_FreeSaveBuffer(char* buffer);
void		G2API_GiveMeVectorFromMatrix(mdxaBone_t *boltMatrix, Eorientations_e flags, vec3_t vec);
void G2API_ListBones(CGhoul2Info* ghlInfo, int frame);
void G2API_ListSurfaces(CGhoul2Info* ghlInfo);
void		G2API_LoadGhoul2Models(CGhoul2Info_v &ghoul2, char *buffer);
void		G2API_LoadSaveCodeDestructGhoul2Info(CGhoul2Info_v &ghoul2);
void G2API_ResetRagDoll(CGhoul2Info_v& ghoul2);
void		G2API_SetBoltInfo(CGhoul2Info_v &ghoul2, int modelIndex, int boltInfo);
void G2API_SetBoltInfo(CGhoul2Info_v& ghoul2, int modelIndex, int boltInfo);
void G2API_SetGhoul2ModelIndexes(CGhoul2Info_v& ghoul2, qhandle_t* modelList, qhandle_t* skinList);
void		G2API_SetRagDoll(CGhoul2Info_v &ghoul2,CRagDollParams *parms);
void G2API_SetTime(int currentTime, int clock);
void RestoreGhoul2InfoArray();
void SaveGhoul2InfoArray();

// From tr_ghoul2.cpp
bool G2API_SkinlessModel(CGhoul2Info_v& ghoul2, int modelIndex);
const char* G2API_GetModelName(CGhoul2Info_v& ghoul2, int modelIndex);
int	G2API_Ghoul2Size(CGhoul2Info_v& ghoul2);
void		G2_ConstructGhoulSkeleton( CGhoul2Info_v &ghoul2,const int frameNum,bool checkForNewOrigin,const vec3_t scale);
void RemoveBoneCache(CBoneCache* boneCache);

#ifdef _G2_GORE
int			G2API_GetNumGoreMarks(CGhoul2Info_v& ghoul2, int modelIndex);
void		G2API_AddSkinGore(CGhoul2Info_v &ghoul2,SSkinGoreData &gore);
void		G2API_ClearSkinGore ( CGhoul2Info_v &ghoul2 );
#endif // _SOF2
