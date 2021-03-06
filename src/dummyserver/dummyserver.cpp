//
// Copyright (c) 2007-2009 2009 Universidade Aveiro - Instituto de
// Telecomunicacoes Polo Aveiro
// This file is part of ODTONE - Open Dot Twenty One.
//
// This software is distributed under a license. The full license
// agreement can be found in the file LICENSE in this distribution.
// This software may not be copied, modified, sold or distributed
// other than expressed in the named license agreement.
//
// This software is distributed without any warranty.
//
// Author:     Simao Reis <sreis@av.it.pt>
//

#include <odtone/debug.hpp>
#include <odtone/mih/message.hpp>
#include <odtone/mih/indication.hpp>
#include <odtone/mih/response.hpp>
#include <odtone/mih/request.hpp>
#include <odtone/mih/types/information.hpp>
#include <odtone/mih/types/capabilities.hpp>
#include <odtone/mih/types/capabilities.hpp>
#include <odtone/mih/config.hpp>
#include <odtone/mih/tlv.hpp>
#include <odtone/mih/tlv_types.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

#include <odtone/mih/types/information_elements.hpp>

#include "../mihf/udp_listener.hpp"

using namespace odtone;
using boost::asio::ip::udp;

static void process_message(meta_message_ptr& msg)
{
	std::cout << "MIH message: Service=" << msg->sid()
		  << " Opcode=" << msg->opcode()
		  << " Action=" << msg->aid()
		  << std::endl;

	if (msg->mid() == mih::response::get_information) {
		mih::ir_bin_data_list bin_data_list;

		*msg >> mih::response(mih::response::get_information)
			& mih::tlv_info_resp_bin_data_list(bin_data_list);

		mih::ir_bin_data bd = bin_data_list[0];

		mih::ie_container_list_of_networks l;
		bd.input() & mih::tlv_ie_container_list_of_networks(l);
	}
}


int main(int argc, char **argv)
{
	boost::asio::io_service io_service;

	dispatch_t dispatch = boost::bind(process_message, _1);
	mihf::udp_listener listen(io_service, boost::asio::ip::udp::v4(),
				  "127.0.0.1", 4000, dispatch);

	listen.start();
	io_service.run();

	return 0;
}
