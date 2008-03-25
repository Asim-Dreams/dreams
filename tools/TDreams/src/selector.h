/*****************************************************************************
 *
 * @author Oscar Rosell
 *
 * Copyright (C) 2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 *****************************************************************************/
#ifndef SELECTOR_H
#define SELECTOR_H

#include <asim/dralListener.h>
#include <libxml/parser.h>
#include <vector>
#include <string>

class LIFETIME_CLASS;

class SELECTOR
{
  public:
    virtual ~SELECTOR(){};
    virtual SELECTOR* Clone() const { return NULL; };
    virtual std::string id() const {return "selector";};
    virtual bool isValid(LIFETIME_CLASS* lifetime) const
    {
        return false;
    };
    virtual bool readConfig(xmlNode* config){return true;};
};

class NODE_INFO
{
  public:
    static const UINT32 NULL_NODE_INSTANCE = UINT32(-1);
    NODE_INFO();

    bool readConfig(xmlNode* config);
    void setNode(std::string node);
    void setInstance(UINT32 instance);
    UINT32 getNodeId() const;

  protected:
    std::string node_name_;
    UINT32 instance_;
    UINT32 node_id_;
};

class ENTER_NODE_SELECTOR : public SELECTOR, NODE_INFO
{
  public:
    virtual ENTER_NODE_SELECTOR* Clone() const;
    virtual std::string id() const {return theId_;}; 
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;
    bool readConfig(xmlNode* config);

  private:
    static const char* const theId_;
};

class FIRST_STAGE_SELECTOR : public SELECTOR, NODE_INFO
{
  public:
    virtual FIRST_STAGE_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;
    virtual std::string id() const {return theId_;}; 
    bool readConfig(xmlNode* config);

  private:
    static const char* const theId_;
};

class LAST_STAGE_SELECTOR : public SELECTOR, NODE_INFO
{
  public:
    virtual LAST_STAGE_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;
    virtual std::string id() const {return theId_;}; 
    bool readConfig(xmlNode* config);

  private:
    static const char* const theId_;
};

class ITEM_TAG_SELECTOR : public SELECTOR
{
  public:
    virtual ITEM_TAG_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;
    virtual std::string id() const {return theId_;};
    bool readConfig(xmlNode* config);

  private:
    static const char* const theId_;
    std::string name_;
    std::string value_;
    UINT64 num_value_;
};

class ALL_SELECTOR : public SELECTOR
{
  public:
    virtual ALL_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const
    {
        return true;
    }
    virtual std::string id() const {return theId_;}; 
  private:
    static const char* const theId_;
};

class AND_SELECTOR : public SELECTOR
{
  public:
    AND_SELECTOR(SELECTOR* selector_a, SELECTOR* selector_b):
        selector_a_(selector_a),
        selector_b_(selector_b)
    {}
    virtual ~AND_SELECTOR()
    {
        // We assume property of selectors
        delete selector_a_;
        delete selector_b_;
    }
    virtual AND_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;

  private:
    SELECTOR* selector_a_;
    SELECTOR* selector_b_;
};

template <class T>
class NOT_SELECTOR : public SELECTOR
{
  public:
    virtual NOT_SELECTOR<T>* Clone() const
    {
        T* t_clone = selector_.Clone();
        NOT_SELECTOR<T>* clone = new NOT_SELECTOR<T>;
        clone->selector_ = *t_clone;
        return clone;
    };
    virtual bool isValid(LIFETIME_CLASS* lifetime) const
    {
        return !selector_.isValid(lifetime);
    }
    bool readConfig(xmlNode* config)
    {
        return selector_.readConfig(config);
    }

  private:
    T selector_;
};

class COMPOSITE_SELECTOR : public SELECTOR
{
  public:
    virtual ~COMPOSITE_SELECTOR();
    virtual COMPOSITE_SELECTOR* Clone() const;
    virtual bool isValid(LIFETIME_CLASS* lifetime) const;
    virtual std::string id() const {return name_;}; 
    bool readConfig(xmlNode* config);

  private:
    bool addInfo(xmlNode* info_node);
    void selfRegister();

    std::vector<SELECTOR*> selectors_;
    std::string info_;
    std::string name_;
};

void 
RegisterDefaultSelectors();


#endif
