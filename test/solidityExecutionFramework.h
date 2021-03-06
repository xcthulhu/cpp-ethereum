
/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * @author Christian <c@ethdev.com>
 * @date 2014
 * Framework for executing Solidity contracts and testing them against C++ implementation.
 */

#pragma once

#include <string>
#include <tuple>
#include <boost/test/unit_test.hpp>
#include <libethereum/State.h>
#include <libethereum/Executive.h>
#include <libsolidity/CompilerStack.h>

namespace dev
{
/// Provides additional overloads for toBigEndian to encode arguments and return values.
inline bytes toBigEndian(byte _value) { return bytes({_value}); }
inline bytes toBigEndian(bool _value) { return bytes({byte(_value)}); }

namespace solidity
{
namespace test
{

class ExecutionFramework
{
public:
	ExecutionFramework() { g_logVerbosity = 0; }

	bytes const& compileAndRun(std::string const& _sourceCode, u256 const& _value = 0, std::string const& _contractName = "")
	{
		dev::solidity::CompilerStack compiler;
		compiler.compile(_sourceCode, m_optimize);
		bytes code = compiler.getBytecode(_contractName);
		sendMessage(code, true, _value);
		BOOST_REQUIRE(!m_output.empty());
		return m_output;
	}

	bytes const& callContractFunction(byte _index, bytes const& _data = bytes(), u256 const& _value = 0)
	{
		sendMessage(bytes(1, _index) + _data, false, _value);
		return m_output;
	}

	template <class... Args>
	bytes const& callContractFunction(byte _index, Args const&... _arguments)
	{
		return callContractFunction(_index, argsToBigEndian(_arguments...));
	}

	template <class CppFunction, class... Args>
	void testSolidityAgainstCpp(byte _index, CppFunction const& _cppFunction, Args const&... _arguments)
	{
		bytes solidityResult = callContractFunction(_index, _arguments...);
		bytes cppResult = callCppAndEncodeResult(_cppFunction, _arguments...);
		BOOST_CHECK_MESSAGE(solidityResult == cppResult, "Computed values do not match."
							"\nSolidity: " + toHex(solidityResult) + "\nC++:      " + toHex(cppResult));
	}

	template <class CppFunction, class... Args>
	void testSolidityAgainstCppOnRange(byte _index, CppFunction const& _cppFunction,
									   u256 const& _rangeStart, u256 const& _rangeEnd)
	{
		for (u256 argument = _rangeStart; argument < _rangeEnd; ++argument)
		{
			bytes solidityResult = callContractFunction(_index, argument);
			bytes cppResult = callCppAndEncodeResult(_cppFunction, argument);
			BOOST_CHECK_MESSAGE(solidityResult == cppResult, "Computed values do not match."
								"\nSolidity: " + toHex(solidityResult) + "\nC++:      " + toHex(cppResult) +
								"\nArgument: " + toHex(toBigEndian(argument)));
		}
	}

private:
	template <class FirstArg, class... Args>
	bytes argsToBigEndian(FirstArg const& _firstArg, Args const&... _followingArgs) const
	{
		return toBigEndian(_firstArg) + argsToBigEndian(_followingArgs...);
	}

	bytes argsToBigEndian() const { return bytes(); }

	template <class CppFunction, class... Args>
	auto callCppAndEncodeResult(CppFunction const& _cppFunction, Args const&... _arguments)
	-> typename std::enable_if<std::is_void<decltype(_cppFunction(_arguments...))>::value, bytes>::type
	{
		_cppFunction(_arguments...);
		return bytes();
	}
	template <class CppFunction, class... Args>
	auto callCppAndEncodeResult(CppFunction const& _cppFunction, Args const&... _arguments)
	-> typename std::enable_if<!std::is_void<decltype(_cppFunction(_arguments...))>::value, bytes>::type
	{
		return toBigEndian(_cppFunction(_arguments...));
	}

	void sendMessage(bytes const& _data, bool _isCreation, u256 const& _value = 0)
	{
		m_state.addBalance(m_sender, _value); // just in case
		eth::Executive executive(m_state, eth::LastHashes(), 0);
		eth::Transaction t = _isCreation ? eth::Transaction(_value, m_gasPrice, m_gas, _data, 0, KeyPair::create().sec())
										 : eth::Transaction(_value, m_gasPrice, m_gas, m_contractAddress, _data, 0, KeyPair::create().sec());
		bytes transactionRLP = t.rlp();
		try
		{
			// this will throw since the transaction is invalid, but it should nevertheless store the transaction
			executive.setup(&transactionRLP);
		}
		catch (...) {}
		if (_isCreation)
		{
			BOOST_REQUIRE(!executive.create(m_sender, _value, m_gasPrice, m_gas, &_data, m_sender));
			m_contractAddress = executive.newAddress();
			BOOST_REQUIRE(m_contractAddress);
			BOOST_REQUIRE(m_state.addressHasCode(m_contractAddress));
		}
		else
		{
			BOOST_REQUIRE(m_state.addressHasCode(m_contractAddress));
			BOOST_REQUIRE(!executive.call(m_contractAddress, m_contractAddress, m_sender, _value, m_gasPrice, &_data, m_gas, m_sender));
		}
		BOOST_REQUIRE(executive.go());
		m_state.noteSending(m_sender);
		executive.finalize();
		m_output = executive.out().toVector();
	}

protected:
	bool m_optimize = false;
	Address m_sender;
	Address m_contractAddress;
	eth::State m_state;
	u256 const m_gasPrice = 100 * eth::szabo;
	u256 const m_gas = 1000000;
	bytes m_output;
};

}
}
} // end namespaces

