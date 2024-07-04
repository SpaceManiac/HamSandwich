#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#***************************************************************************
#                                  _   _ ____  _
#  Project                     ___| | | |  _ \| |
#                             / __| | | | |_) | |
#                            | (__| |_| |  _ <| |___
#                             \___|\___/|_| \_\_____|
#
# Copyright (C) Daniel Stenberg, <daniel@haxx.se>, et al.
#
# This software is licensed as described in the file COPYING, which
# you should have received as part of this distribution. The terms
# are also available at https://curl.se/docs/copyright.html.
#
# You may opt to use, copy, modify, merge, publish, distribute and/or sell
# copies of the Software, and permit persons to whom the Software is
# furnished to do so, under the terms of the COPYING file.
#
# This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
# KIND, either express or implied.
#
# SPDX-License-Identifier: curl
#
###########################################################################
#
import difflib
import filecmp
import logging
import os
import shutil
import pytest

from testenv import Env, CurlClient, VsFTPD


log = logging.getLogger(__name__)


@pytest.mark.skipif(condition=not Env.has_vsftpd(), reason=f"missing vsftpd")
# rustsl: transfers sometimes fail with "received corrupt message of type InvalidContentType"
# sporadic, never seen when filter tracing is on
@pytest.mark.skipif(condition=Env.curl_uses_lib('rustls-ffi'), reason=f"rustls unreliable here")
class TestVsFTPD:

    SUPPORTS_SSL = True

    @pytest.fixture(autouse=True, scope='class')
    def vsftpds(self, env):
        if not TestVsFTPD.SUPPORTS_SSL:
            pytest.skip('vsftpd does not seem to support SSL')
        vsftpds = VsFTPD(env=env, with_ssl=True)
        if not vsftpds.start():
            vsftpds.stop()
            TestVsFTPD.SUPPORTS_SSL = False
            pytest.skip('vsftpd does not seem to support SSL')
        yield vsftpds
        vsftpds.stop()

    def _make_docs_file(self, docs_dir: str, fname: str, fsize: int):
        fpath = os.path.join(docs_dir, fname)
        data1k = 1024*'x'
        flen = 0
        with open(fpath, 'w') as fd:
            while flen < fsize:
                fd.write(data1k)
                flen += len(data1k)
        return flen

    @pytest.fixture(autouse=True, scope='class')
    def _class_scope(self, env, vsftpds):
        if os.path.exists(vsftpds.docs_dir):
            shutil.rmtree(vsftpds.docs_dir)
        if not os.path.exists(vsftpds.docs_dir):
            os.makedirs(vsftpds.docs_dir)
        self._make_docs_file(docs_dir=vsftpds.docs_dir, fname='data-1k', fsize=1024)
        self._make_docs_file(docs_dir=vsftpds.docs_dir, fname='data-10k', fsize=10*1024)
        self._make_docs_file(docs_dir=vsftpds.docs_dir, fname='data-1m', fsize=1024*1024)
        self._make_docs_file(docs_dir=vsftpds.docs_dir, fname='data-10m', fsize=10*1024*1024)

    def test_31_01_list_dir(self, env: Env, vsftpds: VsFTPD, repeat):
        curl = CurlClient(env=env)
        url = f'ftp://{env.ftp_domain}:{vsftpds.port}/'
        r = curl.ftp_ssl_get(urls=[url], with_stats=True)
        r.check_stats(count=1, http_status=226)
        lines = open(os.path.join(curl.run_dir, 'download_#1.data')).readlines()
        assert len(lines) == 4, f'list: {lines}'

    # download 1 file, no SSL
    @pytest.mark.parametrize("docname", [
        'data-1k', 'data-1m', 'data-10m'
    ])
    def test_31_02_download_1(self, env: Env, vsftpds: VsFTPD, docname, repeat):
        curl = CurlClient(env=env)
        srcfile = os.path.join(vsftpds.docs_dir, f'{docname}')
        count = 1
        url = f'ftp://{env.ftp_domain}:{vsftpds.port}/{docname}?[0-{count-1}]'
        r = curl.ftp_ssl_get(urls=[url], with_stats=True)
        r.check_stats(count=count, http_status=226)
        self.check_downloads(curl, srcfile, count)

    @pytest.mark.parametrize("docname", [
        'data-1k', 'data-1m', 'data-10m'
    ])
    def test_31_03_download_10_serial(self, env: Env, vsftpds: VsFTPD, docname, repeat):
        curl = CurlClient(env=env)
        srcfile = os.path.join(vsftpds.docs_dir, f'{docname}')
        count = 10
        url = f'ftp://{env.ftp_domain}:{vsftpds.port}/{docname}?[0-{count-1}]'
        r = curl.ftp_ssl_get(urls=[url], with_stats=True)
        r.check_stats(count=count, http_status=226)
        self.check_downloads(curl, srcfile, count)

    @pytest.mark.parametrize("docname", [
        'data-1k', 'data-1m', 'data-10m'
    ])
    def test_31_04_download_10_parallel(self, env: Env, vsftpds: VsFTPD, docname, repeat):
        curl = CurlClient(env=env)
        srcfile = os.path.join(vsftpds.docs_dir, f'{docname}')
        count = 10
        url = f'ftp://{env.ftp_domain}:{vsftpds.port}/{docname}?[0-{count-1}]'
        r = curl.ftp_ssl_get(urls=[url], with_stats=True, extra_args=[
            '--parallel'
        ])
        r.check_stats(count=count, http_status=226)
        self.check_downloads(curl, srcfile, count)

    def check_downloads(self, client, srcfile: str, count: int,
                        complete: bool = True):
        for i in range(count):
            dfile = client.download_file(i)
            assert os.path.exists(dfile)
            if complete and not filecmp.cmp(srcfile, dfile, shallow=False):
                diff = "".join(difflib.unified_diff(a=open(srcfile).readlines(),
                                                    b=open(dfile).readlines(),
                                                    fromfile=srcfile,
                                                    tofile=dfile,
                                                    n=1))
                assert False, f'download {dfile} differs:\n{diff}'



