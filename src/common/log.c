/*
 * Sylpheed -- a GTK+ based, lightweight, and fast e-mail client
 * Copyright (C) 1999-2002 Hiroyuki Yamamoto
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <glib.h>

#include "defs.h"
#include "utils.h"

static FILE *log_fp = NULL;

void set_log_file(const gchar *filename)
{
	if (log_fp) return;
	log_fp = fopen(filename, "wb");
	if (!log_fp)
		FILE_OP_ERROR(filename, "fopen");
}

void close_log_file(void)
{
	if (log_fp) {
		fclose(log_fp);
		log_fp = NULL;
	}
}

static guint log_verbosity_count = 0;

void log_verbosity_set(gboolean verbose)
{
	if (verbose)
		log_verbosity_count++;
	else if (log_verbosity_count > 0)
		log_verbosity_count--;
}

#define TIME_LEN	11

void log_print(const gchar *format, ...)
{
	va_list args;
	gchar buf[BUFFSIZE + TIME_LEN];
	time_t t;

	time(&t);
	strftime(buf, TIME_LEN + 1, "[%H:%M:%S] ", localtime(&t));

	va_start(args, format);
	g_vsnprintf(buf + TIME_LEN, BUFFSIZE, format, args);
	va_end(args);

	if (debug_get_mode()) fputs(buf, stdout);
/*	FIXME:
	callback for gui logging

	log_window_append(buf, LOG_NORMAL); */
	if (log_fp) {
		fputs(buf, log_fp);
		fflush(log_fp);
	}
/*	FIXME:
	callback for gui logging

	if (log_verbosity_count)
		statusbar_puts_all(buf + TIME_LEN); */
}

void log_message(const gchar *format, ...)
{
	va_list args;
	gchar buf[BUFFSIZE + TIME_LEN];
	time_t t;

	time(&t);
	strftime(buf, TIME_LEN + 1, "[%H:%M:%S] ", localtime(&t));

	va_start(args, format);
	g_vsnprintf(buf + TIME_LEN, BUFFSIZE, format, args);
	va_end(args);

	if (debug_get_mode()) g_message("%s", buf + TIME_LEN);
/*	FIXME:
	callback for gui logging

	log_window_append(buf + TIME_LEN, LOG_MSG); */
	if (log_fp) {
		fwrite(buf, TIME_LEN, 1, log_fp);
		fputs("* message: ", log_fp);
		fputs(buf + TIME_LEN, log_fp);
		fflush(log_fp);
	}
/*	FIXME:
	callback for gui logging

	statusbar_puts_all(buf + TIME_LEN); */
}

void log_warning(const gchar *format, ...)
{
	va_list args;
	gchar buf[BUFFSIZE + TIME_LEN];
	time_t t;

	time(&t);
	strftime(buf, TIME_LEN + 1, "[%H:%M:%S] ", localtime(&t));

	va_start(args, format);
	g_vsnprintf(buf + TIME_LEN, BUFFSIZE, format, args);
	va_end(args);

	g_warning("%s", buf);
/*	FIXME:
	callback for gui logging

	log_window_append(buf + TIME_LEN, LOG_WARN); */
	if (log_fp) {
		fwrite(buf, TIME_LEN, 1, log_fp);
		fputs("** warning: ", log_fp);
		fputs(buf + TIME_LEN, log_fp);
		fflush(log_fp);
	}
}

void log_error(const gchar *format, ...)
{
	va_list args;
	gchar buf[BUFFSIZE + TIME_LEN];
	time_t t;

	time(&t);
	strftime(buf, TIME_LEN + 1, "[%H:%M:%S] ", localtime(&t));

	va_start(args, format);
	g_vsnprintf(buf + TIME_LEN, BUFFSIZE, format, args);
	va_end(args);

	g_warning("%s", buf);
/*	FIXME:
	callback for gui logging

	log_window_append(buf + TIME_LEN, LOG_ERROR); */
	if (log_fp) {
		fwrite(buf, TIME_LEN, 1, log_fp);
		fputs("*** error: ", log_fp);
		fputs(buf + TIME_LEN, log_fp);
		fflush(log_fp);
	}
}
