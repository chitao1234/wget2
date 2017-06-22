/*
 * Copyright(c) 2017 Free Software Foundation, Inc.
 *
 * This file is part of libwget.
 *
 * Libwget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Libwget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libwget.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "wget.h"
#include "fuzzer.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
	wget_cookie_db_t *db;
	wget_cookie_t *cookie;
	wget_iri_t *iri;

	if (size > 1000) // same as max_len = 10000 in .options file
		return 0;

	char *in = (char *) malloc(size + 1);

	assert(in != NULL);

	// 0 terminate
	memcpy(in, data, size);
	in[size] = 0;

	free(wget_cookie_to_setcookie(NULL));
	wget_cookie_store_cookie(NULL, NULL);

	db = wget_cookie_db_init(NULL);
	wget_cookie_set_keep_session_cookies(db, 1);

	wget_cookie_parse_setcookie(in, &cookie);
	if (cookie) {
		wget_cookie_store_cookie(db, cookie);
		wget_cookie_check_psl(db, cookie);
		iri = wget_iri_parse("x.y", "iso-8859-1");
		wget_cookie_normalize(iri, cookie);
		wget_cookie_store_cookie(db, cookie);
		free(wget_cookie_create_request_header(db, iri));
		wget_iri_free(&iri);
	}
	free(wget_cookie_to_setcookie(cookie));

	free(cookie);
//	wget_cookie_free(&cookie);
	wget_cookie_db_free(&db);

	free(in);

	return 0;
}