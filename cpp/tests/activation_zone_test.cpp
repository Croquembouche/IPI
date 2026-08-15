#include "test_support.hpp"

#include "ipi/activation/activation.hpp"

#include <chrono>

int main() {
    return ipi::tests::run_test("activation_zone_state_machine", [] {
        using namespace std::chrono_literals;
        using namespace ipi::activation;
        using ipi::api::GeoPoint;
        using ipi::api::GeometryType;
        using ipi::api::Timestamp;

        ActivationZone zone;
        zone.zoneId = "zone-1";
        zone.revision = 1;
        zone.geofence.type = GeometryType::CIRCLE;
        zone.geofence.points = {GeoPoint{42.0, -83.0, std::nullopt}};
        zone.geofence.radiusMeters = 100.0;
        zone.validFrom = Timestamp{100s};
        zone.expiresAt = Timestamp{200s};
        zone.enterHysteresisMeters = 5.0;
        zone.exitHysteresisMeters = 5.0;
        zone.minimumDwell = 2s;
        zone.actions = {ActivationAction::MODEL_PULL,
                        ActivationAction::TELEMETRY_PUSH};

        ActivationZoneTracker tracker;
        ipi::tests::expect(tracker.set_zone(zone, Timestamp{110s}),
                           "first zone revision should install");
        ipi::tests::expect(!tracker.set_zone(zone, Timestamp{111s}),
                           "same zone revision should be idempotent");

        const auto steady0 = std::chrono::steady_clock::time_point{10s};
        auto update = tracker.update(GeoPoint{42.002, -83.0, std::nullopt},
                                     steady0, Timestamp{120s});
        ipi::tests::expect(update.state == ActivationState::OUTSIDE && !update.stateChanged,
                           "vehicle should begin outside");

        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 1s, Timestamp{121s});
        ipi::tests::expect(update.state == ActivationState::ENTERING && update.stateChanged,
                           "qualified entry should start dwell");
        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 2s, Timestamp{122s});
        ipi::tests::expect(update.state == ActivationState::ENTERING && !update.stateChanged,
                           "entry must remain pending until dwell completes");
        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 3s, Timestamp{123s});
        ipi::tests::expect(update.state == ActivationState::ACTIVE && update.stateChanged,
                           "entry dwell should activate once");
        ipi::tests::expect(update.enabledActions.size() == 2,
                           "active transition should expose only configured actions");
        const auto activeSequence = update.sequence;
        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 4s, Timestamp{124s});
        ipi::tests::expect(!update.stateChanged && update.sequence == activeSequence,
                           "stable active samples must not emit duplicate transitions");

        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 5s, Timestamp{125s}, false);
        ipi::tests::expect(update.state == ActivationState::FALLBACK && update.stateChanged,
                           "infrastructure loss while active must enter fallback");
        ipi::tests::expect(update.enabledActions.empty(),
                           "fallback must not retain infrastructure actions");

        update = tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                                steady0 + 6s, Timestamp{126s}, true);
        ipi::tests::expect(update.state == ActivationState::ENTERING,
                           "infrastructure recovery inside zone should re-enter through dwell");
        (void)tracker.update(GeoPoint{42.0, -83.0, std::nullopt},
                             steady0 + 8s, Timestamp{128s}, true);
        ipi::tests::expect(tracker.state() == ActivationState::ACTIVE,
                           "recovered entry should activate after dwell");

        update = tracker.update(GeoPoint{42.002, -83.0, std::nullopt},
                                steady0 + 9s, Timestamp{129s});
        ipi::tests::expect(update.state == ActivationState::EXITING,
                           "qualified exit should start dwell");
        update = tracker.update(GeoPoint{42.002, -83.0, std::nullopt},
                                steady0 + 11s, Timestamp{131s});
        ipi::tests::expect(update.state == ActivationState::OUTSIDE,
                           "exit dwell should return outside");
    });
}
