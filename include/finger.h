#ifndef finger_h___
#define finger_h___

#include "utilities.h" 
#include "target_object.h"
#include <iostream>

namespace ICR
{
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/*! 
 *  \brief Describes a volume centered on a vertex of the target object; The id's of all vertices within this
 *  volume form a patch on the target object's surface
 *
 *  \details Each ICR::Finger of a ICR::Grasp has an inclusion rule as a member. Those inclusion
 *   rules can differ between fingers. If they do, a separate list of patches will be created for
 *   each finger during initialization of the grasp. If the flag
 *   ICR::InclusionRule::filter_inside_points_ is true, all patches corresponding to the respective
 *   finger only contain the center-point and border points. Here, the border is defined by those
 *   points having neighbors which don't qualify for patch inclusion. Right now, the inclusion rule
 *   is governed only by the scalar ICR::InclusionRule::rule_parameter_ which gives the radius of a
 *   sphere centered at a given center-point on the target object. One possibility to generalize
 *   this struct would be to change the rule parameter to a 3x3 Matrix parametrizing an ellipsoid
 *   and add an additional rotation matrix to give the orientation of this ellipsoid w.r.t the
 *   target object. An even more sophisticated possibility would be to formulate the rule parameter
 *   as a mesh describing the respective finger tip with a given offset. Maybe also change the name
 *   to ICR::PatchRule or something like that to avoid confusion with
 *   ICR::IndependentContactRegions::searchZoneInclusionTest and
 *   ICR::IndependentContactRegions::primitiveSearchZoneInclusionTest.
 *  
 */
struct InclusionRule
{
  double rule_parameter_;
  RuleType rule_type_;
  bool filter_inside_points_;

  InclusionRule();
  friend std::ostream& operator<<(std::ostream &stream,InclusionRule const& inclusion_rule);
/*! 
 * Returns true if the volume parametrized by ICR::InclusionRule::rule_parameter_, which is centered at center_point, contains test_point
 */
  bool inclusionTest(ContactPoint const* center_point, ContactPoint const* test_point)const;
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/*! 
 *  \brief A std::list<ICR::Node> acts as a FIFO list during the breadth-first computation of which
 *  points qualify for patch inclusion in ICR::Patch(uint centerpoint_id,TargetObject const& obj,
 *  InclusionRule const& rule)

 * \details If ICR::Node::inside_patch_ is true, the contact point referenced by
 * ICR::Node::contact_point_ is inside the patch, i.e., all of the neighbors of this point qualify
 * for patch inclusion as well.
 */
struct Node
{
  ContactPoint* contact_point_;
  bool inside_patch_;

  Node();
  Node(ContactPoint* contact_point);
  friend std::ostream& operator<<(std::ostream &stream, Node const& node);
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/*! 
 *  \brief Contains the id's of points forming a patch on the target object's surface in ICR::Patch::patch_ids_
 */
struct Patch
{
  IndexList patch_ids_;
  
  Patch();
/*! 
 *  centerpoint_id states the id of the ICR::ContactPoint at which the patch is centered
 *
 *\param a centerpoint_id in the range of 0 - ICR::TargetObject#num_cp_
 *
 */
  Patch(uint centerpoint_id);
/*! 
 *  centerpoint_id states the id of the ICR::ContactPoint at which the patch is centered
 *
 *\param a centerpoint_id in the range of 0 - ICR::TargetObject#num_cp_
 *\param a valid target object
 *\param a valid inclusion rule
 *
 *Performs a breadth-first exploration of obj's contact points with the contact point given by
 *centerpoint_id as root node. The criterion to qualify a point for inclusion in the patch
 *is that rule.inclusionTest() is satisfied.
 *
 */
  Patch(uint centerpoint_id, TargetObject const& obj, InclusionRule const& rule);
  friend std::ostream& operator<<(std::ostream &stream,Patch const& patch);
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
/*! 
 *  \brief Describes contact force magnitude, friction cone discretization, friction coefficient,
 *  torsional friction coefficient, contact type, contact model type and the patch inclusion rule
 *  for a ICR::Finger
 */
class FingerParameters
{
 private:

  double force_magnitude_;
  uint disc_;
  double mu_0_;
  double mu_T_;
  ContactType contact_type_;
  ModelType model_type_;
  InclusionRule inclusion_rule_;

 public:

  FingerParameters();
  FingerParameters(FingerParameters const& src);
  friend std::ostream& operator<<(std::ostream &stream,FingerParameters const& param);
  FingerParameters& operator=(FingerParameters const& src);
  ~FingerParameters();

  void setFrictionlessContact(double force_magnitude);
  void setFrictionalContact(double force_magnitude,uint disc,double mu_0);
  void setSoftFingerContact(double force_magnitude,uint disc,double mu_0, double mu_T);
  void setContactModelType(ModelType model_type);
  void setInclusionRule(InclusionRule const& inclusion_rule);
  void setInclusionRuleType(RuleType rule_type);
  void setInclusionRuleParameter(uint rule_parameter);
  void setInclusionRuleFilterPatch(bool filter_inside_points);

  double getForceMagnitude()const;
  uint getDisc()const;
  double getMu0()const;
  double getMuT()const;
  ContactType getContactType()const;
  ModelType getModelType()const;
  InclusionRule const* getInclusionRule()const;
  RuleType getInclusionRuleType()const;
  uint getInclusionRuleParameter()const;
  bool getInclusionRuleFilterPatch()const;
};
//---------------------------------------------------------------------
//---------------------------------------------------------------------
/*! 
 *  \brief Holds a ICR::ContactPoint#id_ describing where on the target object the finger is
 *  centered as well as pointers to a list of patches, an ICR::OWS and a ICR::PointContactModel
 */
class Finger
{
 private:
  
  PointContactModel* c_model_;
  OWSPtr ows_;
  PatchListPtr patches_;
  uint centerpoint_id_;
  bool initialized_;

 public:

  Finger();
  Finger(FingerParameters const& param);
  Finger(Finger const& src);
  Finger& operator=(Finger const& src);
  friend std::ostream& operator<<(std::ostream& stream,Finger const& finger);
  ~Finger();
 
  void init(uint centerpoint_id, const PatchListPtr patches, const OWSPtr ows);
  void setCenterPointId(uint centerpoint_id);
  Patch const* getCenterPointPatch() const;
  Patch const* getPatch(uint id)const;
  const OWSPtr getOWS()const;
  const PatchListPtr getPatches()const;
  PointContactModel const* getContactModel()const; 
  PointContactModel* getContactModel(); 
  uint getCenterPointId()const;
  bool isInitialized()const;

};
//---------------------------------------------------------------------
//---------------------------------------------------------------------
}//namespace ICR
#endif
