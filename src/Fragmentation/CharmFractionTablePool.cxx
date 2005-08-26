//____________________________________________________________________________
/*!

\class    genie::CharmFractionTablePool

\brief    Singleton class to load & serve charm fraction tables.

\author   Costas Andreopoulos <C.V.Andreopoulos@rl.ac.uk>
          CCLRC, Rutherford Appleton Laboratory

\created  June 15, 2005

*/
//____________________________________________________________________________

#include <iostream>
#include <string>

#include "libxml/parser.h"
#include "libxml/xmlmemory.h"

#include <TSystem.h>

#include "Messenger/Messenger.h"
#include "Utils/StringUtils.h"
#include "Utils/XmlParserUtils.h"
#include "Fragmentation/CharmFractionTablePool.h"

using std::string;

using namespace genie;

//____________________________________________________________________________
CharmFractionTablePool * CharmFractionTablePool::fInstance = 0;
//____________________________________________________________________________
CharmFractionTablePool::CharmFractionTablePool()
{
  if( ! this->LoadTables() ) {
   LOG("CFracTab", pERROR) << "CharmFractionTablePool initialization failed!";
  }
  fInstance =  0;
}
//____________________________________________________________________________
CharmFractionTablePool::~CharmFractionTablePool()
{
  LOG("CFracTab", pINFO) << "Deleting all charm fraction tables";
  map<string, CharmFractionTable *>::iterator titer;
  for(titer = fTablePool.begin(); titer != fTablePool.end(); ++titer) {
    CharmFractionTable * table = titer->second;
    if(table) {
      delete table;
      table = 0;
    }
  }
  fTablePool.clear();
  fInstance = 0;
}
//____________________________________________________________________________
CharmFractionTablePool * CharmFractionTablePool::Instance()
{
  if(fInstance == 0) {

    static CharmFractionTablePool::Cleaner cleaner;
    cleaner.DummyMethodAndSilentCompiler();

    fInstance = new CharmFractionTablePool;
  }
  return fInstance;
}
//____________________________________________________________________________
bool CharmFractionTablePool::LoadTables(void)
{
  bool loaded = true;

  //-- get base GENIE base directory from the environment
  string genie_base_dir = string( gSystem->Getenv("GENIE") );

  //-- build the full pathnames for possible pdg data file locations
  string path = genie_base_dir + string("/config/charm_fraction_tables.xml");

  LOG("CFracTab", pINFO)  << "\n *** Loading charm fractions from " << path;

  bool is_accessible = ! (gSystem->AccessPathName( path.c_str() ));

  if ( is_accessible ) {
      XmlParserStatus_t status = this->ParseXMLTables(path.c_str());
      if(status != kXmlOK) {
         LOG("CFracTab", pWARN)
                           << "\n *** " << XmlParserStatus::AsString(status);
         loaded = false;
      }
  } else {
      LOG("CFracTab", pWARN) << "\n *** Charm Fractions could not be loaded";
      loaded = false;
  }
  return loaded;
};
//____________________________________________________________________________
XmlParserStatus_t CharmFractionTablePool::ParseXMLTables(string filename)
{
  LOG("CFracTab", pDEBUG) << "Retrieving data from XML file: " << filename;

  xmlDocPtr xml_doc = xmlParseFile(filename.c_str());
  if(xml_doc == NULL) return kXmlNotParsed;

  xmlNodePtr xml_cur = xmlDocGetRootElement(xml_doc);
  if(xml_cur==NULL) return kXmlEmpty;

  if( xmlStrcmp(xml_cur->name, (const xmlChar *)
                           "charm_fraction_table") ) return kXmlInvalidRoot;

  // parsing single charm fractions table
  string name = string_utils::TrimSpaces(
                             XmlParserUtils::GetAttribute(xml_cur, "name"));

  LOG("CFracTab", pDEBUG) << "Reading charm fraction table: " << name;

  xmlNodePtr xml_cur_ebin = xml_cur->xmlChildrenNode; // <energy_bin>'s

  // loop over all <charm_fraction_table> node children nodes
  while (xml_cur_ebin != NULL) {

     // enter everytime you find an <energy_bin> tag
     if( (!xmlStrcmp(xml_cur_ebin->name, (const xmlChar *) "energy_bin")) ) {

        string semin = string_utils::TrimSpaces(
                               XmlParserUtils::GetAttribute(xml_cur, "min"));
        string semax = string_utils::TrimSpaces(
                               XmlParserUtils::GetAttribute(xml_cur, "max"));

        double emin  = atof( semin.c_str() );
        double emax  = atof( semax.c_str() );

        xmlNodePtr xml_cur_frac = xml_cur_ebin->xmlChildrenNode;

        while (xml_cur_frac != NULL) {

             string val = XmlParserUtils::TrimSpaces(
                             xmlNodeListGetString(xml_doc, xml_cur_frac, 1));

             string pdg = string_utils::TrimSpaces(
                           XmlParserUtils::GetAttribute(xml_cur, "pdg_code"));

             assert( !xmlStrcmp(xml_cur_frac->name,
                                              (const xmlChar *) "fraction") );

             double frac = atof(val.c_str());
             int    pdgc = atoi(pdg.c_str());

             LOG("CFracTab", pDEBUG) << "Fraction(PDG: " << pdgc
                         << ", emin = " << emin << ", emax = " << emax
                                                           << ") = " << frac;

             xml_cur_frac = xml_cur_frac->next;
        } // [end of] loop over tags within <fraction> ... </fraction> tags

        xmlFree(xml_cur_frac);

     } // [end of] found & parsing an <energy_bin> section

     xml_cur_ebin = xml_cur_ebin->next;
  } // [end of] loop over tags within <charm_fraction_table>...</> tags

  xmlFree(xml_cur);
  xmlFree(xml_cur_ebin);

  return kXmlOK;
}
//____________________________________________________________________________

