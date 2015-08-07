/* CPU load average functions
 *
 * Copyright (C) 2015  Christian Lockley <clockley1@gmail.com>
 * Copyright (C) 2015  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "wdt.h"

/* Defaults are a bit low */
double load_warn   = .7;
double load_reboot = .8;

/* Local variables */
static uev_t watcher;


static double check(void)
{
	double load[3] = { 0 };

	if (getloadavg(load, 3) == -1)
		return -1;

	return (load[0] + load[1]) / 2.0;
}

static void cb(uev_t *w, void *arg, int events)
{
	double load = check();

	DEBUG("Current loadavg %f", load);
	if (load > load_warn && load < load_reboot) {
		WARN("System load average very high!");
	} else if (load > load_reboot) {
		ERROR("System load too high, rebooting system ...");
		wdt_reboot(w, arg, events);
	}
}

/* Every 2T seconds we check loadavg */
int loadavg_init(uev_ctx_t *ctx, int T)
{
	return uev_timer_init(ctx, &watcher, cb, NULL, 2 * T, 2 * T);
}

int loadavg_set_level(double load)
{
	if (load <= 0) {
		ERROR("Load average argument must be greater than zero.");
		return 1;
	}

	load_warn   = load;
	load_reboot = load + 0.1;

	return 0;
}

/**
 * Local Variables:
 *  c-file-style: "linux"
 *  indent-tabs-mode: t
 *  version-control: t
 * End:
 */