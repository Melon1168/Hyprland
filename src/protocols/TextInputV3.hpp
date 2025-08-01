#pragma once

#include <vector>
#include <cstdint>
#include <string>
#include "WaylandProtocol.hpp"
#include "text-input-unstable-v3.hpp"
#include "../helpers/signal/Signal.hpp"
#include "../helpers/math/Math.hpp"

class CWLSurfaceResource;

class CTextInputV3 {
  public:
    CTextInputV3(SP<CZwpTextInputV3> resource_);
    ~CTextInputV3();

    void       enter(SP<CWLSurfaceResource> surf);
    void       leave(SP<CWLSurfaceResource> surf);
    void       preeditString(const std::string& text, int32_t cursorBegin, int32_t cursorEnd);
    void       commitString(const std::string& text);
    void       deleteSurroundingText(uint32_t beforeLength, uint32_t afterLength);
    void       sendDone();

    bool       good();

    wl_client* client();

    struct {
        CSignalT<> onCommit;
        CSignalT<> enable;
        CSignalT<> disable;
        CSignalT<> reset;
        CSignalT<> destroy;
    } m_events;

    struct SState {
        struct {
            bool        updated = false;
            std::string text    = "";
            uint32_t    cursor  = 0;
            uint32_t    anchor  = 0;
        } surrounding;

        struct {
            bool                         updated = false;
            zwpTextInputV3ContentHint    hint    = ZWP_TEXT_INPUT_V3_CONTENT_HINT_NONE;
            zwpTextInputV3ContentPurpose purpose = ZWP_TEXT_INPUT_V3_CONTENT_PURPOSE_NORMAL;
        } contentType;

        struct {
            bool updated = false;
            CBox cursorBox;
        } box;

        struct {
            bool isEnablePending  = false;
            bool isDisablePending = false;
            bool value            = false;
        } enabled;

        zwpTextInputV3ChangeCause cause = ZWP_TEXT_INPUT_V3_CHANGE_CAUSE_INPUT_METHOD;

        void                      reset();
    };

    SState m_pending;
    SState m_current;

  private:
    SP<CZwpTextInputV3> m_resource;

    int                 m_serial = 0;
};

class CTextInputV3Protocol : public IWaylandProtocol {
  public:
    CTextInputV3Protocol(const wl_interface* iface, const int& ver, const std::string& name);

    virtual void bindManager(wl_client* client, void* data, uint32_t ver, uint32_t id);

    struct {
        CSignalT<WP<CTextInputV3>> newTextInput;
    } m_events;

  private:
    void onManagerResourceDestroy(wl_resource* res);
    void destroyTextInput(CTextInputV3* input);
    void onGetTextInput(CZwpTextInputManagerV3* pMgr, uint32_t id, wl_resource* seat);

    //
    std::vector<UP<CZwpTextInputManagerV3>> m_managers;
    std::vector<SP<CTextInputV3>>           m_textInputs;

    friend class CTextInputV3;
};

namespace PROTO {
    inline UP<CTextInputV3Protocol> textInputV3;
};
