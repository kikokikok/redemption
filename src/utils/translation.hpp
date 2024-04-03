/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Product name: redemption, a FLOSS RDP proxy
 *   Copyright (C) Wallix 2010-2013
 *   Author(s): Christophe Grosjean, Meng Tan, Jonathan Poelen
 */


#pragma once

#include "configs/autogen/enums.hpp" // Language
#include "cxx/diagnostic.hpp"
#include "utils/trkey.hpp"

#include "system/i18n/message_translator_gettext.hpp"


struct Translation
{
private:
    Language lang = Language::en;
    Language prev_lang = Language::en;
    mutable bool translation_context_set = false;
    mutable i18n::MessageTranslatorGettext message_translator;

    Translation() = default;

    inline void reset_message_translator_context() const
    {
        if (!translation_context_set || this->lang != this->prev_lang)
        {
            if (!translation_context_set)
            {
                translation_context_set = true;
            }
            else
            {
                this->message_translator.clear_context();
            }

            switch (this->lang)
            {
                case Language::en:
                    this->message_translator.set_context("en"_zv);
                    break;
                case Language::fr:
                    this->message_translator.set_context("fr"_zv);
                    break;
            }
        }
    }

    [[nodiscard]]
    inline zstring_view _translate(zstring_view text) const
    {
        reset_message_translator_context();
        return this->message_translator.get_translated_text(text);
    }

public:
    Translation(Translation const&) = delete;
    void operator=(Translation const&) = delete;

    static Translation& getInstance()
    {
        static Translation instance;
        return instance;
    }

    void set_lang(Language lang)
    {
        this->prev_lang = this->lang;
        this->lang = lang;
    }

    [[nodiscard]] zstring_view translate(TrKey k) const
    {
        return _translate(k.translation);
    }

    template<class T, class... Ts>
    auto translate_fmt(char* s, std::size_t n, TrKeyFmt<T> k, Ts const&... xs) const
    -> decltype(T::check_printf_result(s, n, xs...))
    {
        REDEMPTION_DIAGNOSTIC_PUSH()
        REDEMPTION_DIAGNOSTIC_GCC_IGNORE("-Wformat-nonliteral")
        return std::snprintf(s, n, _translate(k.translation).c_str(), xs...);
        REDEMPTION_DIAGNOSTIC_POP()
    }
};


inline zstring_view TR(TrKey k, Language lang)
{
    Translation::getInstance().set_lang(lang);
    return Translation::getInstance().translate(k);
}

template<class T, class... Ts>
int TR_fmt(char* s, std::size_t n, TrKeyFmt<T> k, Language lang, Ts const&... xs)
{
    Translation::getInstance().set_lang(lang);
    return Translation::getInstance().translate_fmt(s, n, k, xs...);
}

struct Translator
{
    explicit Translator(Language lang)
      : lang(lang)
    {}

    zstring_view operator()(TrKey const & k) const
    {
        return TR(k, this->lang);
    }

    template<class T, class... Ts>
    int fmt(char* s, std::size_t n, TrKeyFmt<T> k, Ts const&... xs) const
    {
        return TR_fmt(s, n, k, this->lang, xs...);
    }

private:
    Language lang;
};
