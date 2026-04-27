#include "test_support.hpp"

#include "ipi/api/experiment_logging.hpp"

int main() {
    return ipi::tests::run_test("experiment_logging", [] {
        ipi::api::ExperimentContext context;
        context.runId = "run-1";
        context.conditionId = "cond-1";
        context.requestIdBase = "req";

        const char* argv[] = {"prog",
                              "--condition-label",
                              "private-5g-stressed",
                              "--network-load-level",
                              "heavy",
                              "--service-success",
                              "false",
                              "--vehicle-outcome-name",
                              "path_error",
                              "--vehicle-outcome-value",
                              "0.42",
                              "--vehicle-outcome-unit",
                              "m"};
        constexpr int argc = static_cast<int>(sizeof(argv) / sizeof(argv[0]));
        int index = 1;
        while (index < argc) {
            const auto consumed =
                ipi::api::consume_experiment_context_arg(context, argv[index], index, argc, const_cast<char**>(argv));
            ipi::tests::expect(consumed, "expected experiment arg to be consumed");
            ++index;
        }

        ipi::tests::expect(context.conditionLabel == "private-5g-stressed", "condition label should parse");
        ipi::tests::expect(context.networkLoadLevel == "heavy", "network load should parse");
        ipi::tests::expect(!context.serviceSuccess, "service success should parse as false");
        ipi::tests::expect(context.vehicleOutcomeName == "path_error", "vehicle outcome name should parse");
        ipi::tests::expect(context.vehicleOutcomeValue == "0.42", "vehicle outcome value should parse");
        ipi::tests::expect(context.vehicleOutcomeUnit == "m", "vehicle outcome unit should parse");
        ipi::tests::expect(ipi::api::default_condition_label("radio", "idle") == "radio-baseline",
                           "radio condition label should default to radio-baseline");
        ipi::tests::expect(ipi::api::default_condition_label("mqtt", "idle") == "private-5g-baseline",
                           "idle 5G condition label should default to private-5g-baseline");
        ipi::tests::expect(ipi::api::default_condition_label("tcp", "heavy") == "private-5g-stressed",
                           "loaded 5G condition label should default to private-5g-stressed");

        ipi::api::ExperimentLogRecord record;
        record.emitterRole = "5g-sender";
        record.emitTimeNs = 100;
        record.runId = "run-1";
        record.conditionId = "cond-1";
        record.conditionLabel = "private-5g-baseline";
        record.serviceType = "guided-planning";
        record.transport = "tcp";
        record.requestId = "req-1";
        record.accepted = true;
        record.serviceSuccess = true;
        record.vehicleOutcomeName = "path_error";
        record.vehicleOutcomeValue = "0.42";
        record.vehicleOutcomeUnit = "m";
        record.detail = "ok";

        const auto csv = ipi::api::experiment_log_to_csv(record);
        ipi::tests::expect(csv.find("req-1") != std::string::npos, "csv should contain request id");
        ipi::tests::expect(csv.find("path_error") != std::string::npos, "csv should contain vehicle outcome");

        const auto text = ipi::api::experiment_log_to_text(record);
        ipi::tests::expect(text.find("service=guided-planning") != std::string::npos,
                           "text log should contain service type");
        ipi::tests::expect(text.find("request=req-1") != std::string::npos,
                           "text log should contain request id");
    });
}
