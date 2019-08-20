#pragma once

#include <chipmunk/chipmunk.h>
#include <chipmunk/chipmunk_structs.h>
//#include <chipmunk/chipmunk_types.h>


inline cpVect vtoc(const Vec2d &v) {
  return cpv(v.x, v.y);
}

inline Vec2d ctov(const cpVect &v) {
  return Vec2d(v.x, v.y);
}

static unsigned int lockedCategoryBitMask = 1<<31;
static unsigned int allCategories = -1;
static unsigned int noCategories = 0;

static cpShapeFilter collisionFilterLockedOnly = {0, lockedCategoryBitMask, 0};
static cpShapeFilter collisionFilterDefault = {0, ~lockedCategoryBitMask, allCategories};
static cpShapeFilter collisionFilterLocked = {0, allCategories, allCategories};
static cpShapeFilter collisionFilterFindLocked = {0, lockedCategoryBitMask, lockedCategoryBitMask};
