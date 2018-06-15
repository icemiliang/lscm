#ifndef FACE_NORMAL_TRAIT_H
#define FACE_NORMAL_TRAIT_H

#include "Trait.h"

namespace MeshLib
{
#define f_normal(f) trait<FaceNormalTrait,Face>(f).normal()

class FaceNormalTrait : public Trait
{
public:
	//!  Constructor.
	FaceNormalTrait(){ };
	//!  Destructor.
	~FaceNormalTrait(){ };
	//!  Read method.
	void read(){};
	//!  Write method.
	void write(){};

	//!  Get normal.
	Point & normal(){ return m_normal;};

public:
	//!  SceneFaceTrait normal.
	Point m_normal;

};

} //namespace MeshLib

#endif //FACE_NORMAL_TRAIT_H
