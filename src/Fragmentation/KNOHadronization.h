//____________________________________________________________________________
/*!

\class    genie::KNOHadronization

\brief    The KNO hadronization model.
          This hadronization scheme is similar to the one originally used
          in NeuGEN by G.Barr, G.F.Pearce, H.Gallagher et al. \n
          Improvements were made by C.Andreopoulos, H.Gallagher, T.Yang. \n

          Both the 'historical' version and the new versions are supported.
          See the algorithms configuration for details.

          Is a concrete implementation of the HadronizationModelI interface.

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          CCLRC, Rutherford Appleton Laboratory

\created  August 17, 2004

\cpright  Copyright (c) 2003-2006, GENIE Neutrino MC Generator Collaboration
          All rights reserved.
          For the licensing terms see $GENIE/USER_LICENSE.
*/
//____________________________________________________________________________

#ifndef _KNO_HADRONIZATION_H_
#define _KNO_HADRONIZATION_H_

#include <TGenPhaseSpace.h>

#include "Fragmentation/HadronizationModelBase.h"

class TF1;

namespace genie {

class DecayModelI;
class Spline;

class KNOHadronization : public HadronizationModelBase {

public:
  KNOHadronization();
  KNOHadronization(string config);
  virtual ~KNOHadronization();

  //! implement the HadronizationModelI interface
  void           Initialize       (void)                                    const;
  TClonesArray * Hadronize        (const Interaction* )                     const;
  double         Weight           (void)                                    const;
  PDGCodeList *  SelectParticles  (const Interaction*)                      const;
  TH1D *         MultiplicityProb (const Interaction*, Option_t* opt = "")  const;

  //! overload the Algorithm::Configure() methods to load private data
  //! members from configuration options
  void Configure(const Registry & config);
  void Configure(string config);

private:

  //! private methods & mutable parameters

  void          LoadConfig            (void);
  bool          AssertValidity        (const Interaction * i)        const;
  PDGCodeList * GenerateFSHadronCodes (int mult, int maxQ, double W) const;
  int           GenerateBaryonPdgCode (int mult, int maxQ)           const;
  int           HadronShowerCharge    (const Interaction * )         const;
  double        KNO                   (int nu, int nuc, double z)    const;
  double        AverageChMult         (int nu, int nuc, double W)    const;
  void          HandleDecays          (TClonesArray * particle_list) const;
  double        ReWeightPt2           (const PDGCodeList & pdgcv)    const;

  TClonesArray* DecayMethod1    (double W, const PDGCodeList & pdgv) const;
  TClonesArray* DecayMethod2    (double W, const PDGCodeList & pdgv) const;
  TClonesArray* DecayBackToBack (double W, const PDGCodeList & pdgv) const;

  bool PhaseSpaceDecay(
             TClonesArray & pl, TLorentzVector & pd, 
	                     const PDGCodeList & pdgv, int offset=0) const;

  mutable TGenPhaseSpace fPhaseSpaceGenerator;
  mutable double         fWeight;

  //! Configuration parameters
  //! Note: additional configuration parameters common to all hadronizers
  //! (Wcut,Rijk,...) are declared one layer down in the inheritance tree

  const DecayModelI * fDecayer;  ///< decay algorithm
  bool     fForceNeuGenLimit;    ///< force upper hadronic multiplicity to NeuGEN limit
  bool     fUseLegacyKNOSpline;  ///< use legacy spline instead of Levy
  bool     fUseBaryonXfPt2Param; ///< Generate baryon xF,pT2 from experimental parameterization?
  bool     fReWeightDecays;      ///< Reweight phase space decays?
  bool     fForceDecays;         ///< force decays of unstable hadrons produced?
  bool     fForceMinMult;        ///< force minimum multiplicity if (at low W) generated less?
  bool     fGenerateWeighted;    ///< generate weighted events?
  double   fPhSpRwA;             ///< parameter for phase space decay reweighting
  double   fPpi0;                ///< pi0 pi0 production probability
  double   fPpic;                ///< pi+ pi- production probability
  double   fPKc;                 ///< K+  K- production probability
  double   fPK0;                 ///< K0  K0bar production probability
  double   fAvp;                 ///< offset in average multiplicity relation for vp
  double   fAvn;                 ///< offset in average multiplicity relation for vn
  double   fAvbp;                ///< offset in average multiplicity relation for vbp
  double   fAvbn;                ///< offset in average multiplicity relation for vbn
  double   fB;                   ///< slope  in average multiplicity relation
  double   fCvp;                 ///< Levy function parameter for vp
  double   fCvn;                 ///< Levy function parameter for vn
  double   fCvbp;                ///< Levy function parameter for vbp
  double   fCvbn;                ///< Levy function parameter for vbn
  TF1 *    fBaryonXFpdf;         ///< baryon xF PDF
  TF1 *    fBaryonPT2pdf;        ///< baryon pT^2 PDF
  Spline * fKNO;                 ///< legacy KNO distribution (superseded by the Levy func)
};

}         // genie namespace

#endif    // _KNO_HADRONIZATION_H_

