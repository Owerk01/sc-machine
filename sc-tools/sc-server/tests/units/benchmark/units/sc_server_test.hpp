/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include <sc-memory/sc_memory.hpp>

extern "C"
{
#include <sc-core/sc_memory_params.h>
}

#include "sc-client/sc_client.hpp"

#include "sc-server-impl/sc_server_impl.hpp"

class TestScServer
{
public:
  static inline std::string const & SC_SERVER_KB_BIN = "sc-server-test-kb-bin";

  void Initialize(size_t objectsNum = 0)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.storage = SC_SERVER_KB_BIN.c_str();

    params.clear = SC_FALSE;

    ScMemory::LogMute();

    std::random_device random_device;
    std::uniform_int_distribution<> distribution(MIN_TEST_SERVER_PORT, MAX_TEST_SERVER_PORT);
    std::mt19937 generator(random_device());

    ScMemory::Initialize(params);
    m_server = std::make_unique<ScServerImpl>("127.0.0.1", distribution(generator), SC_TRUE);
    m_server->ClearChannels();
    m_server->Run();
    ScMemory::LogUnmute();

    InitContext();
    Setup(objectsNum);
  }

  void Shutdown()
  {
    DestroyContext();

    ScMemory::LogMute();
    ScMemory::Shutdown(false);
    m_server->Stop();
    m_server = nullptr;
    ScMemory::LogUnmute();
  }

  void InitContext()
  {
    m_ctx = std::make_unique<ScMemoryContext>();
  }

  void DestroyContext()
  {
    if (m_ctx != nullptr)
      m_ctx->Destroy();
  }

  void WaitServer()
  {
    while (m_server->IsWorkable())
      ;
  }

  virtual void Setup(size_t objectsNum) {}

  static std::unique_ptr<ScServer> m_server;

protected:
  static std::unique_ptr<ScMemoryContext> m_ctx;

  sc_int const MAX_TEST_SERVER_PORT = 30000;
  sc_int const MIN_TEST_SERVER_PORT = 20000;
};

std::unique_ptr<ScServer> TestScServer::m_server{};
std::unique_ptr<ScMemoryContext> TestScServer::m_ctx{};
