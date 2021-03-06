//=============================================================================
// Brief   : Generic Network Interface
// Authors : Bruno Santos <bsantos@av.it.pt>
//
//
// Copyright (C) 2009-2010 Universidade Aveiro - Instituto de Telecomunicacoes Polo Aveiro
//
// This file is part of ODTONE - Open Dot Twenty One.
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//=============================================================================

#ifndef LINK_SAP_INTERFACE_GENERIC__HPP_
#define LINK_SAP_INTERFACE_GENERIC__HPP_

///////////////////////////////////////////////////////////////////////////////
#include "../base.hpp"
#include <odtone/string.hpp>
#include <boost/utility.hpp>
#include <boost/array.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/intrusive/rbtree.hpp>
#include <boost/type_traits/is_pod.hpp>
#include <boost/mpl/equal.hpp>
#include <odtone/mih/types/link.hpp>
#include <algorithm>
#include <cstring>

///////////////////////////////////////////////////////////////////////////////
namespace link_sap { namespace nic {

///////////////////////////////////////////////////////////////////////////////
class if_id {

#ifdef WIN32
	typedef boost::array<uint32, 4> value_type;
#else
	typedef int                     value_type;
#endif

public:
	if_id()
		: _id()
	{ }

	explicit if_id(const value_type& id)
		: _id(id)
	{ }

	template<class T>
	explicit if_id(const T* pod)
	{
		ODTONE_STATIC_ASSERT(sizeof(T) == sizeof(value_type), "T must be the same size as if_id underlying type");
		ODTONE_STATIC_ASSERT(boost::is_pod<T>::value, "T must be a POD");
		_id = reinterpret_cast<const value_type&>(*pod);
	}

	bool operator<(if_id const& lhs) const
	{
		return (_id < lhs._id);
	}

	bool operator<=(if_id const& lhs) const
	{
		return (_id <= lhs._id);
	}

	bool operator>(if_id const& lhs) const
	{
		return (_id > lhs._id);
	}

	bool operator>=(if_id const& lhs) const
	{
		return (_id >= lhs._id);
	}

	bool operator==(if_id const& lhs) const
	{
		return (_id == lhs._id);
	}

	bool operator!=(if_id const& lhs) const
	{
		return (_id != lhs._id);
	}

private:
	value_type _id;
};

///////////////////////////////////////////////////////////////////////////////
class interface : boost::noncopyable {
	friend class interface_map;

public:
	interface(if_id const& id, odtone::mih::link_type type);
	virtual ~interface();

	if_id const&                  id() const        { return _id; }
	odtone::mih::link_type        type() const      { return _type; }
	const boost::tribool&         up() const        { return _up; }
	const odtone::string&         name() const      { return _name; }
	const odtone::mih::link_addr& link_addr() const { return _link_addr; }

	boost::logic::tribool up(const boost::logic::tribool& tb);
	void                  name(const odtone::string& name);
	void                  link_addr(const odtone::mih::link_addr& link_addr);

private:
	boost::intrusive::set_member_hook<> _node;
	odtone::mih::link_type              _type;

protected:
	if_id		           _id;
	boost::logic::tribool  _up;
	odtone::string         _name;
	odtone::mih::link_addr _link_addr;
};

///////////////////////////////////////////////////////////////////////////////
class interface_map {
	typedef boost::intrusive::member_hook<interface, boost::intrusive::set_member_hook<>, &interface::_node>
		hook_option;

	struct compare {
		bool operator()(const interface& a, const interface& b) const
		{
			return a._id < b._id;
		}

		bool operator()(const interface& a, if_id const& id) const
		{
			return a._id < id;
		}

		bool operator()(if_id const& id, const interface& b) const
		{
			return id < b._id;
		}
	};
	typedef boost::intrusive::compare<compare> compare_option;

	typedef boost::intrusive::rbtree<interface, compare_option, hook_option> map;

public:
	typedef map::iterator       iterator;
	typedef map::const_iterator const_iterator;

	interface_map();
	~interface_map();

	std::pair<iterator, bool> insert(interface& i);
	void erase(const_iterator i);
	void erase(if_id const& id);

	iterator       find(if_id const& id);
	const_iterator find(if_id const& id) const;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

private:
	map _map;
};

///////////////////////////////////////////////////////////////////////////////
} /* namespace nic */ } /* namespace link_sap */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* LINK_SAP_INTERFACE_GENERIC__HPP_ */
