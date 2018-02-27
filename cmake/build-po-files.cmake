# Copyright (c) 2017 Aleix Pol Gonzalez <aleixpol@kde.org>
# Copyright (c) 2017 Harald Sitter <sitter@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the University nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

file(GLOB_RECURSE pofiles RELATIVE "${PO_DIR}" "${PO_DIR}/**.po")

foreach(pofile IN LISTS pofiles)
    if (NOT pofile MATCHES "_qt.po")
        get_filename_component(name ${pofile} NAME_WE)
        get_filename_component(langdir ${pofile} DIRECTORY)
        set(dest ${COPY_TO}/${langdir}/LC_MESSAGES)
        file(MAKE_DIRECTORY ${dest})

        message(STATUS "building... ${pofile} to ${name}.mo" )
        execute_process(
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${dest}/${name}.mo ${PO_DIR}/${pofile}
            RESULT_VARIABLE code
        )
        if(code)
            message(FATAL_ERROR "failed at generating ${name}.mo")
        endif()
    endif()
endforeach()
