/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2001 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include "sortdict.h"
#include "config.h"

#include "section.h"

class PageInfo
{
  public:
    PageInfo(const char *f, int l,const char *n,const char *d,const char *t) :
      defFileName(f), defLine(l), name(n), 
      doc(d), title(t), todoId(0), testId(0),inGroup(0), sectionDict(0) {}

    // where the page definition was found
    QCString defFileName;
    int defLine;

    // contents of the page
    QCString name;
    QCString doc;
    QCString title;

    // external reference? if so then this is the tag file name
    QCString reference;
    QCString fileName;

    // functions to get a uniform interface with Definitions
    QCString getOutputFileBase() const 
    { return fileName; }
    bool isReference() const { return !reference.isEmpty(); }
    QCString getReference() const { return reference; }
    
    void addSections(QList<QCString> *anchorList)
    {
      if (anchorList)
      {
        QCString *s=anchorList->first();
        while (s)
        {
          SectionInfo *si=0;
          if (!s->isEmpty() && (si=Doxygen::sectionDict[*s]))
          {
            //printf("Add section `%s' to definition `%s'\n",
            //    si->label.data(),n.data());
            if (sectionDict==0) 
            {
              sectionDict = new SectionDict(17);
            }
            if (sectionDict->find(*s)==0)
            {
              sectionDict->insert(*s,si);
            }
            si->pageRef = this;
            si->fileName = fileName;
          }
          s=anchorList->next();
        }
      }
    }
    
    void writeDocAnchorsToTagFile()
    {
      if (!Config::genTagFile.isEmpty() && sectionDict)
      {
        QDictIterator<SectionInfo> sdi(*sectionDict);
        SectionInfo *si;
        for (;(si=sdi.current());++sdi)
        {
          Doxygen::tagFile << "    <docanchor>" << si->label << "</docanchor>" << endl;
        }
      }
    }

    // ids
    int todoId;
    int testId;

    // is this page part of a group
    GroupDef *inGroup;
    SectionDict *sectionDict;
};

class PageSDict : public SDict<PageInfo>
{
  public:
    PageSDict(int size) : SDict<PageInfo>(size) {}
   ~PageSDict() {}
    int compareItems(GCI i1,GCI i2)
    {
      return stricmp(((PageInfo *)i1)->name,((PageInfo *)i2)->name);
    }
};

// typedef is to get around Solaris compile problem
typedef SDict<PageInfo>::Iterator _SDictPageInfoIterator;

class PageSDictIterator : public _SDictPageInfoIterator
{
  public:
    PageSDictIterator(const PageSDict &d) : _SDictPageInfoIterator(d) {}
   ~PageSDictIterator() {} 
};
