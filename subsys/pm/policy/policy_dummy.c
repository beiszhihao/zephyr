/*
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <pm/state.h>
#include <pm/policy.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(pm, CONFIG_PM_LOG_LEVEL);

#define STATE_ACTIVE \
	(struct pm_state_info){PM_STATE_ACTIVE, 0, 0}

static const struct pm_state_info pm_dummy_states[] =
	PM_STATE_INFO_LIST_FROM_DT_CPU(DT_NODELABEL(cpu0));

struct pm_state_info pm_policy_next_state(uint8_t cpu, int32_t ticks)
{
	static struct pm_state_info cur_pm_state_info;
	int i = (int)cur_pm_state_info.state;
	uint8_t states_len = ARRAY_SIZE(pm_dummy_states);

	ARG_UNUSED(cpu);

	if (states_len == 0) {
		/* No power states to go through. */
		return STATE_ACTIVE;
	}

	do {
		i = (i + 1) % states_len;

		if (!pm_constraint_get(
			    pm_dummy_states[i].state)) {
			continue;
		}

		cur_pm_state_info = pm_dummy_states[i];

		LOG_DBG("Selected power state: %u", pm_dummy_states[i].state);

		return pm_dummy_states[i];
	} while (pm_dummy_states[i].state != cur_pm_state_info.state);

	LOG_DBG("No suitable power state found!");
	return STATE_ACTIVE;
}
