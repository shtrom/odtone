//=============================================================================
// Brief   : MIH Binary Information Query Response Types
// Authors : Bruno Santos <bsantos@av.it.pt>
// ----------------------------------------------------------------------------
// ODTONE - Open Dot Twenty One
//
// Copyright (C) 2009-2010 Universidade de Aveiro
// Copyrigth (C) 2009-2010 Instituto de Telecomunicações - Pólo de Aveiro
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//=============================================================================

#ifndef ODTONE_MIH_TYPES_BIN_RESPONSE__HPP_
#define ODTONE_MIH_TYPES_BIN_RESPONSE__HPP_

///////////////////////////////////////////////////////////////////////////////
#include <odtone/base.hpp>
#include <odtone/exception.hpp>
#include <odtone/mih/tlv.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace odtone { namespace mih {

class ir_bin_iarchive;
class ir_bin_oarchive;

///////////////////////////////////////////////////////////////////////////////
//
// TODO: need to improve this with a generic IE element, so this can be turned into a std::vector
//
class ir_bin_data {
	friend class ir_bin_iarchive;
	friend class ir_bin_oarchive;

	struct unspecified_bool_t {
		void true_() {}
	};
	typedef void (unspecified_bool_t::*unspecified_bool)();

public:
	ir_bin_data()
		: _cnt(0)
	{ }

	void serialize(iarchive& ar)
	{
		_cnt = ar.list_length();

		_ar.clear();
		for (uint i = 0; i < _cnt; ++i) {
			uint pos;
			uint end;

			pos = ar.position();
			ar.position(pos + 4);
			end = ar.list_length();
			end += ar.position();
			_ar.append(ar.begin() + pos, ar.begin() + end);
			ar.position(end);
		}
	}

	void serialize(oarchive& ar)
	{
		ar.list_length(_cnt);
		ar.append(_ar.begin(), _ar.end());
	}

	ir_bin_iarchive input();
	ir_bin_oarchive output();

	operator unspecified_bool()
	{
		return _cnt ? &unspecified_bool_t::true_ : 0;
	}

	bool operator!()
	{
		return !_cnt;
	}

private:
	archive _ar;
	uint    _cnt;
};

typedef std::vector<ir_bin_data> ir_bin_data_list;

///////////////////////////////////////////////////////////////////////////////
struct ir_bin_iarchive_error : virtual public exception {
	ir_bin_iarchive_error() : exception("odtone::mih::ir_bin_iarchive: invalid number of elements")
	{ }
};

///////////////////////////////////////////////////////////////////////////////
class ir_bin_iarchive {
public:
	ir_bin_iarchive(ir_bin_data& data)
		: _ar(data._ar), _cnt(data._cnt), _pos(0)
	{ }

	template<class T>
	typename boost::enable_if<is_tlv_type<T>, ir_bin_iarchive&>::type operator&(const T& val)
	{
		if (_pos < _cnt) {
			val.serialize(_ar);
			++_pos;

		} else {
			boost::throw_exception(ir_bin_iarchive_error());
		}

		return *this;
	}

private:
	iarchive _ar;
	uint&    _cnt;
	uint     _pos;
};

class ir_bin_oarchive {
public:
	ir_bin_oarchive(ir_bin_data& data)
		: _ar(data._ar), _cnt(data._cnt)
	{ }

	template<class T>
	typename boost::enable_if<is_tlv_type<T>, ir_bin_oarchive&>::type operator&(const T& val)
	{
		val.serialize(_ar);
		++_cnt;

		return *this;
	}

private:
	oarchive _ar;
	uint&    _cnt;
};

///////////////////////////////////////////////////////////////////////////////
inline ir_bin_iarchive ir_bin_data::input()
{
	return ir_bin_iarchive(*this);
}

inline ir_bin_oarchive ir_bin_data::output()
{
	return ir_bin_oarchive(*this);
}

///////////////////////////////////////////////////////////////////////////////
} /* namespace mih */ } /*namespace odtone */

// EOF ////////////////////////////////////////////////////////////////////////
#endif /* ODTONE_MIH_TYPES_BIN_RESPONSE__HPP_ */
