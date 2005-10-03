//____________________________________________________________________________
/*!

\class   genie::GMCJDriver

\brief   GENIE MC Job Driver (event generation for the input flux & geometry)

\author  Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
         CCLRC, Rutherford Appleton Laboratory

\created May 25, 2005

*/
//____________________________________________________________________________

#ifndef _GENIE_MC_JOB_DRIVER_H_
#define _GENIE_MC_JOB_DRIVER_H_

namespace genie {

class EventRecord;
class GFluxI;
class GeomAnalyzerI;
class GENIE;
class GEVGPool;
class PathLengthList;

class GMCJDriver {

public :

  GMCJDriver();
  ~GMCJDriver();

  //-- configure MC job: set flux and detector geometry

  void UseFluxDriver   (GFluxI * flux);
  void UseGeomAnalyzer (GeomAnalyzerI * geom);
  void UseSplines      (bool useLogE = true);
  void Configure       (void);

  //-- generate single neutrino event

  EventRecord * GenerateEvent (void);

private:

  void   Initialize (void);
  double PInt       (double xsec, double pl);

  GFluxI *         fFluxDriver;   ///< [input] neutrino flux driver
  GeomAnalyzerI *  fGeomAnalyzer; ///< [input] detector geometry analyzer
  GEVGPool *       fGPool;        ///< A pool of available GEVGDrivers objects
  double           fPmax;         ///< [computed] Pmax(interaction) given flux and geom
  bool             fUseSplines;
  bool             fUseLogE;
};

}      // genie namespace

#endif // _GENIE_MC_JOB_DRIVER_H_
