const STORAGE_KEY = "edge4avExperimentTracker.v1";

const CAMPAIGN_FIELDS = [
  { key: "primarySite", label: "Primary site", placeholder: "Instrumented intersection or proving-ground site" },
  { key: "avsUsed", label: "AVs used", placeholder: "Vehicle IDs or count" },
  { key: "obuCount", label: "OBU count", placeholder: "Number of active OBUs" },
  { key: "rsuCount", label: "RSU count", placeholder: "Number of active RSUs" },
  { key: "edgeNode", label: "Edge node", placeholder: "Edge server or RSU host name" },
  { key: "private5gDeployment", label: "Private 5G deployment", placeholder: "Lab core, on-site gNB, sliced network, etc." },
  { key: "brokerHost", label: "Broker host", placeholder: "MQTT broker address if used" },
  { key: "timeSyncMethod", label: "Time sync method", placeholder: "PTP, GNSS-disciplined PTP, NTP" },
];

const SERVICE_CLASS_DEFS = [
  {
    id: "intersection-state",
    title: "intersection-state assistance",
    implementationPath: "radio SPaT/MAP-style state and private-5G SPaT probes",
  },
  {
    id: "guided-planning",
    title: "fallback / guided-planning assistance",
    implementationPath: "IPI-CooperativeService",
  },
];

const FIGURE_DEFS = [
  {
    id: "figure-1",
    title: "Figure 1",
    headline: "Baseline service envelope",
    description: "Across radio, private-5G TCP, and private-5G MQTT.",
  },
  {
    id: "figure-2",
    title: "Figure 2",
    headline: "Scaling and fairness",
    description: "Physical plus emulated load with per-client tail behavior.",
  },
  {
    id: "figure-3",
    title: "Figure 3",
    headline: "Crowded-network / QoS stress behavior",
    description: "How tails grow and how QoS shifts the break point.",
  },
  {
    id: "figure-4",
    title: "Figure 4",
    headline: "Edge-offload vs RSU-local compute",
    description: "Whether offload helps or hurts the guided-planning service.",
  },
  {
    id: "figure-5",
    title: "Figure 5",
    headline: "Vehicle-level outcome",
    description: "How communication quality changes measurable AV behavior.",
  },
];

const CHECKLIST_GROUPS = [
  {
    id: "core",
    title: "Core Main-Paper Experiments",
    description: "These are the experiments that need real data before the paper is submission-ready.",
    items: [
      { id: "E1", label: "E1 Baseline service envelope" },
      { id: "E2", label: "E2 Scaling and contention" },
      { id: "E3", label: "E3 Crowded-network / QoS stress tests" },
      { id: "E4", label: "E4 Edge-offload vs RSU-local compute" },
      { id: "E5", label: "E5 Vehicle-level outcome" },
    ],
  },
  {
    id: "supplementary",
    title: "Supplementary Experiments",
    description: "Useful for the supplement or discussion, but not the main paper spine.",
    items: [
      { id: "S1", label: "S1 Payload sensitivity" },
      { id: "S2", label: "S2 Failure detection and recovery" },
      { id: "S3", label: "S3 Cold-start / session resumption" },
      { id: "S4", label: "S4 Security overhead" },
      { id: "S5", label: "S5 RF gradient sensitivity" },
      { id: "S6", label: "S6 Mobility / handover" },
      { id: "S7", label: "S7 Additional queue-discipline ablations" },
    ],
  },
  {
    id: "claims",
    title: "Claims to Avoid Unless You Have Real Data",
    description: "Check only when you have enough evidence to make the claim safely.",
    items: [
      { id: "claim-multivendor", label: "Multi-vendor interoperability" },
      { id: "claim-public-5g", label: "Broad public-5G equivalence" },
      { id: "claim-coop-perception", label: "Cooperative-perception headline claims without a real end-to-end perception path" },
    ],
  },
];

const READINESS_ITEMS = [
  { id: "readiness-baseline", label: "Baseline envelope figure complete" },
  { id: "readiness-scaling", label: "Scaling and fairness figure complete" },
  { id: "readiness-qos", label: "Crowded-network / QoS figure complete" },
  { id: "readiness-offload", label: "Edge-offload vs local-compute figure complete" },
  { id: "readiness-vehicle", label: "Vehicle-level outcome figure complete" },
  { id: "readiness-radio-5g", label: "One clear radio vs 5G comparison" },
  { id: "readiness-qos-compare", label: "One clear QoS vs no-QoS comparison" },
  { id: "readiness-local-offload", label: "One clear local vs offloaded comparison" },
  { id: "readiness-main-claim", label: "Main claim supported without over-claiming public-5G equivalence" },
];

const RUN_LOG_COLUMNS = [
  { key: "date", label: "Date", type: "date" },
  { key: "runId", label: "Run ID", type: "text" },
  { key: "conditionId", label: "Condition ID", type: "text" },
  { key: "conditionLabel", label: "Condition Label", type: "text" },
  { key: "requestId", label: "Request ID", type: "text" },
  { key: "serviceType", label: "Service Type", type: "select", options: ["", "intersection-state", "guided-planning"] },
  { key: "transport", label: "Transport", type: "select", options: ["", "radio", "tcp", "mqtt"] },
  { key: "avId", label: "AV ID", type: "text" },
  { key: "obuId", label: "OBU ID", type: "text" },
  { key: "rsuId", label: "RSU ID", type: "text" },
  { key: "networkLoadLevel", label: "Load", type: "select", options: ["", "idle", "moderate", "heavy", "near-saturation"] },
  { key: "qosProfile", label: "QoS", type: "text" },
  { key: "mobilityState", label: "Mobility", type: "text" },
  { key: "clockSyncState", label: "Clock Sync", type: "text" },
  { key: "nodesUsed", label: "Nodes Used", type: "text" },
  { key: "logFiles", label: "Log Files", type: "text" },
  { key: "accepted", label: "Accepted", type: "select", options: ["", "yes", "no", "partial"] },
  { key: "serviceSuccess", label: "Service Success", type: "select", options: ["", "yes", "no", "partial"] },
  { key: "vehicleOutcome", label: "Vehicle Outcome", type: "text" },
  { key: "notes", label: "Notes", type: "textarea" },
];

const EXPERIMENT_DEFS = [
  {
    id: "E1",
    scope: "core",
    title: "Baseline service envelope",
    goal: "Measure baseline end-to-end envelopes for the two main service classes across the radio path, private-5G TCP, and private-5G MQTT.",
    minimumData: [
      "per-probe logs",
      "sender CSV",
      "receiver CSV",
      "radio bridge CSV for radio runs",
      "summary p50 / p95 / p99",
      "success rate",
      "jitter",
      "per-hop timing when clocks are synchronized",
    ],
    prompts: [
      "Which path has the best baseline tail behavior?",
      "Is MQTT meaningfully worse than TCP in this deployment?",
      "Does guided-planning stay inside the envelope you want to claim?",
    ],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "service", label: "Service" },
      { key: "transport", label: "Transport" },
      { key: "conditionLabel", label: "Label" },
      { key: "load", label: "Load" },
      { key: "qos", label: "QoS" },
      { key: "mobility", label: "Mobility" },
    ],
    editableFields: [
      { key: "samples", label: "Samples", placeholder: "50", inputMode: "numeric" },
      { key: "successRate", label: "Success Rate", placeholder: "100%" },
      { key: "p50Ms", label: "p50 ms", placeholder: "12.4", inputMode: "decimal" },
      { key: "p95Ms", label: "p95 ms", placeholder: "18.1", inputMode: "decimal" },
      { key: "p99Ms", label: "p99 ms", placeholder: "21.7", inputMode: "decimal" },
      { key: "jitterMs", label: "Jitter ms", placeholder: "3.8", inputMode: "decimal" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Clock sync state, outliers, artifacts." },
    ],
    completionFields: ["samples", "successRate", "p50Ms", "p95Ms", "p99Ms"],
    conditions: [
      { conditionId: "radio-spat-baseline", service: "intersection-state", transport: "radio", conditionLabel: "radio-baseline", load: "idle", qos: "default", mobility: "stationary" },
      { conditionId: "p5g-tcp-spat-baseline", service: "intersection-state", transport: "tcp", conditionLabel: "private-5g-baseline", load: "idle", qos: "default", mobility: "stationary" },
      { conditionId: "p5g-mqtt-spat-baseline", service: "intersection-state", transport: "mqtt", conditionLabel: "private-5g-baseline", load: "idle", qos: "default", mobility: "stationary" },
      { conditionId: "p5g-tcp-guided-baseline", service: "guided-planning", transport: "tcp", conditionLabel: "private-5g-baseline", load: "idle", qos: "default", mobility: "stationary" },
      { conditionId: "p5g-mqtt-guided-baseline", service: "guided-planning", transport: "mqtt", conditionLabel: "private-5g-baseline", load: "idle", qos: "default", mobility: "stationary" },
    ],
  },
  {
    id: "E2",
    scope: "core",
    title: "Scaling and contention",
    goal: "Characterize fairness and tail behavior as you move from one physical client to multiple physical plus emulated clients.",
    minimumData: [
      "per-client p95",
      "max/min fairness ratio",
      "drops",
      "sender CPU",
      "receiver CPU",
      "NIC throughput",
      "queue depth if available",
    ],
    prompts: [
      "At what participant count do tails become unstable?",
      "Is fairness acceptable with one vendor stack plus emulated load?",
    ],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "service", label: "Service" },
      { key: "transport", label: "Transport" },
      { key: "participants", label: "Participants" },
      { key: "conditionLabel", label: "Label" },
      { key: "load", label: "Load" },
    ],
    editableFields: [
      { key: "perClientP95Ms", label: "Per-Client p95 ms", placeholder: "24.6" },
      { key: "fairnessRatio", label: "Fairness Ratio", placeholder: "1.08" },
      { key: "dropRate", label: "Drop Rate", placeholder: "0.0%" },
      { key: "cpuSender", label: "Sender CPU", placeholder: "36%" },
      { key: "cpuReceiver", label: "Receiver CPU", placeholder: "29%" },
      { key: "nicThroughput", label: "NIC Throughput", placeholder: "42 Mbps" },
      { key: "queueDepth", label: "Queue Depth", placeholder: "8" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Client mix, emulator settings, fairness anomalies." },
    ],
    completionFields: ["perClientP95Ms", "fairnessRatio", "dropRate"],
    conditions: [
      { conditionId: "scale-guided-n1", service: "guided-planning", transport: "tcp or mqtt", participants: "1", conditionLabel: "private-5g-baseline", load: "idle" },
      { conditionId: "scale-guided-n2", service: "guided-planning", transport: "tcp or mqtt", participants: "2", conditionLabel: "private-5g-stressed", load: "moderate" },
      { conditionId: "scale-guided-n3", service: "guided-planning", transport: "tcp or mqtt", participants: "3", conditionLabel: "private-5g-stressed", load: "heavy" },
      { conditionId: "scale-guided-n6", service: "guided-planning", transport: "tcp or mqtt", participants: "6", conditionLabel: "private-5g-stressed", load: "heavy" },
    ],
  },
  {
    id: "E3",
    scope: "core",
    title: "Crowded-network / QoS stress tests",
    goal: "Show how high-priority AV services degrade as network load increases, and how much QoS helps.",
    minimumData: [
      "p50 / p95 / p99",
      "success rate",
      "where service breaks",
      "throughput counters",
      "RSRP / SINR / portal counters if available",
    ],
    prompts: [
      "Does QoS materially improve the high-priority service tail?",
      "Where does the service envelope break?",
      "What can you cautiously infer about public-network-like contention?",
    ],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "service", label: "Service" },
      { key: "transport", label: "Transport" },
      { key: "conditionLabel", label: "Label" },
      { key: "load", label: "Load" },
      { key: "qos", label: "QoS" },
    ],
    editableFields: [
      { key: "successRate", label: "Success Rate", placeholder: "99.2%" },
      { key: "p50Ms", label: "p50 ms", placeholder: "14.1" },
      { key: "p95Ms", label: "p95 ms", placeholder: "28.4" },
      { key: "p99Ms", label: "p99 ms", placeholder: "37.6" },
      { key: "tailGrowth", label: "Tail Growth vs Idle", placeholder: "2.1x" },
      { key: "throughput", label: "Throughput", placeholder: "48 Mbps" },
      { key: "rfCounters", label: "RF Counters", placeholder: "RSRP -86 dBm / SINR 19 dB" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Break point, scheduler notes, QoS mapping details." },
    ],
    completionFields: ["successRate", "p50Ms", "p95Ms", "p99Ms", "tailGrowth"],
    conditions: [
      { conditionId: "p5g-tcp-idle-default", service: "guided-planning", transport: "tcp", conditionLabel: "private-5g-baseline", load: "idle", qos: "default" },
      { conditionId: "p5g-tcp-heavy-default", service: "guided-planning", transport: "tcp", conditionLabel: "private-5g-stressed", load: "heavy", qos: "default" },
      { conditionId: "p5g-tcp-heavy-qos", service: "guided-planning", transport: "tcp", conditionLabel: "private-5g-stressed", load: "heavy", qos: "5qi-mapped" },
      { conditionId: "p5g-mqtt-idle-default", service: "guided-planning", transport: "mqtt", conditionLabel: "private-5g-baseline", load: "idle", qos: "default" },
      { conditionId: "p5g-mqtt-near-sat-fifo", service: "guided-planning", transport: "mqtt", conditionLabel: "private-5g-stressed", load: "near-saturation", qos: "fifo" },
      { conditionId: "p5g-mqtt-near-sat-qos", service: "guided-planning", transport: "mqtt", conditionLabel: "private-5g-stressed", load: "near-saturation", qos: "5qi-mapped" },
    ],
  },
  {
    id: "E4",
    scope: "core",
    title: "Edge-offload vs RSU-local compute",
    goal: "Prove whether edge offload helps or hurts the fallback / guided-planning service relative to RSU-local handling.",
    minimumData: [
      "end-to-end latency",
      "success rate",
      "degradation under edge load",
      "CPU split across RSU and edge",
    ],
    prompts: [
      "Is offload worth the added network path?",
      "Under what load does local compute beat edge offload?",
    ],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "service", label: "Service" },
      { key: "placement", label: "Placement" },
      { key: "conditionLabel", label: "Label" },
      { key: "load", label: "Load" },
    ],
    editableFields: [
      { key: "successRate", label: "Success Rate", placeholder: "100%" },
      { key: "p50Ms", label: "p50 ms", placeholder: "18.2" },
      { key: "p95Ms", label: "p95 ms", placeholder: "23.0" },
      { key: "p99Ms", label: "p99 ms", placeholder: "29.4" },
      { key: "cpuRsu", label: "CPU RSU", placeholder: "42%" },
      { key: "cpuEdge", label: "CPU Edge", placeholder: "36%" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Placement trade-offs, queueing, failures." },
    ],
    completionFields: ["successRate", "p50Ms", "p95Ms", "p99Ms"],
    conditions: [
      { conditionId: "local-guided-idle", service: "guided-planning", placement: "RSU-local", conditionLabel: "private-5g-baseline", load: "idle" },
      { conditionId: "edge-guided-idle", service: "guided-planning", placement: "edge-offloaded", conditionLabel: "private-5g-baseline", load: "idle" },
      { conditionId: "local-guided-heavy", service: "guided-planning", placement: "RSU-local", conditionLabel: "private-5g-stressed", load: "heavy" },
      { conditionId: "edge-guided-heavy", service: "guided-planning", placement: "edge-offloaded", conditionLabel: "private-5g-stressed", load: "heavy" },
    ],
  },
  {
    id: "E5",
    scope: "core",
    title: "Vehicle-level outcome",
    goal: "Tie communication quality to actual AV behavior with one or two primary vehicle metrics.",
    minimumData: [
      "on-time service success",
      "vehicle metric 1",
      "vehicle metric 2",
      "notes on path deviation, stop-line behavior, fallback completion, or on-time guidance arrival",
    ],
    prompts: [
      "Does degraded network quality produce degraded AV behavior?",
      "Is the effect large enough to justify the infrastructure claim?",
    ],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "service", label: "Service" },
      { key: "networkCondition", label: "Network Condition" },
      { key: "vehicleMetric", label: "Vehicle Metric" },
    ],
    editableFields: [
      { key: "onTimeServiceSuccess", label: "On-Time Service Success", placeholder: "98%" },
      { key: "vehicleMetric1", label: "Vehicle Metric 1", placeholder: "0.24 m path deviation" },
      { key: "vehicleMetric2", label: "Vehicle Metric 2", placeholder: "1 late fallback event" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Driverless run context, route, safety overrides." },
    ],
    completionFields: ["onTimeServiceSuccess", "vehicleMetric1"],
    conditions: [
      { conditionId: "vehicle-guided-baseline", service: "guided-planning", networkCondition: "baseline", vehicleMetric: "" },
      { conditionId: "vehicle-guided-stressed", service: "guided-planning", networkCondition: "stressed", vehicleMetric: "" },
      { conditionId: "vehicle-state-baseline", service: "intersection-state", networkCondition: "baseline", vehicleMetric: "" },
      { conditionId: "vehicle-state-stressed", service: "intersection-state", networkCondition: "stressed", vehicleMetric: "" },
    ],
  },
  {
    id: "S1",
    scope: "supplementary",
    title: "Payload sensitivity",
    goal: "Measure how payload size shifts tails for the same transport path.",
    minimumData: ["payload size", "transport", "p95", "notes"],
    prompts: ["Does payload size materially shift the p95 you report in the paper?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "payloadSize", label: "Payload Size" },
    ],
    editableFields: [
      { key: "transport", label: "Transport", placeholder: "tcp or mqtt" },
      { key: "p95Ms", label: "p95 ms", placeholder: "22.4" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Any transport asymmetry or fragmentation effects." },
    ],
    completionFields: ["transport", "p95Ms"],
    conditions: [
      { conditionId: "payload-0p2kb", payloadSize: "0.2 KB" },
      { conditionId: "payload-1kb", payloadSize: "1 KB" },
      { conditionId: "payload-4kb", payloadSize: "4 KB" },
    ],
  },
  {
    id: "S2",
    scope: "supplementary",
    title: "Failure detection and recovery",
    goal: "Record how quickly failures are detected and how quickly service recovers.",
    minimumData: ["failure type", "time to detect", "time to recover", "notes"],
    prompts: ["Which failure domain is most expensive to recover from?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "failureType", label: "Failure Type" },
    ],
    editableFields: [
      { key: "timeToDetect", label: "Time to Detect", placeholder: "1.8 s" },
      { key: "timeToRecover", label: "Time to Recover", placeholder: "4.2 s" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Observed alarms, retries, route changes." },
    ],
    completionFields: ["timeToDetect", "timeToRecover"],
    conditions: [
      { conditionId: "failure-rsu", failureType: "RSU loss" },
      { conditionId: "failure-edge", failureType: "edge loss" },
    ],
  },
  {
    id: "S3",
    scope: "supplementary",
    title: "Cold-start / session resumption",
    goal: "Characterize registration and resume cost after startup or a transient drop.",
    minimumData: ["resume type", "registration time", "resume time", "notes"],
    prompts: ["Is resume fast enough to avoid a visible service gap?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "resumeType", label: "Resume Type" },
    ],
    editableFields: [
      { key: "registrationTime", label: "Registration Time", placeholder: "740 ms" },
      { key: "resumeTime", label: "Resume Time", placeholder: "290 ms" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Session IDs, retries, broker reconnect behavior." },
    ],
    completionFields: ["registrationTime", "resumeTime"],
    conditions: [
      { conditionId: "resume-cold-start", resumeType: "cold start" },
      { conditionId: "resume-transient-drop", resumeType: "transient drop" },
    ],
  },
  {
    id: "S4",
    scope: "supplementary",
    title: "Security overhead",
    goal: "Measure whether security mode materially shifts latency tails.",
    minimumData: ["security mode", "p95", "overhead vs plaintext", "notes"],
    prompts: ["Does security overhead materially move the claimed operating envelope?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "securityMode", label: "Security Mode" },
    ],
    editableFields: [
      { key: "p95Ms", label: "p95 ms", placeholder: "26.0" },
      { key: "overheadVsPlaintext", label: "Overhead vs Plaintext", placeholder: "+3.8 ms" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "TLS mode, cert path, handshake reuse." },
    ],
    completionFields: ["p95Ms", "overheadVsPlaintext"],
    conditions: [
      { conditionId: "sec-plain", securityMode: "plaintext" },
      { conditionId: "sec-tls", securityMode: "TLS / mTLS" },
    ],
  },
  {
    id: "S5",
    scope: "supplementary",
    title: "RF gradient sensitivity",
    goal: "Capture how RF quality shifts the latency distribution.",
    minimumData: ["RF state", "RSRP", "SINR", "p95", "notes"],
    prompts: ["At what RF quality does the tail start to move sharply?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "rfState", label: "RF State" },
    ],
    editableFields: [
      { key: "rsrp", label: "RSRP", placeholder: "-86 dBm" },
      { key: "sinr", label: "SINR", placeholder: "19 dB" },
      { key: "p95Ms", label: "p95 ms", placeholder: "24.1" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Location, antenna, weather, obstructions." },
    ],
    completionFields: ["rsrp", "sinr", "p95Ms"],
    conditions: [
      { conditionId: "rf-near", rfState: "near" },
      { conditionId: "rf-mid", rfState: "mid" },
      { conditionId: "rf-far", rfState: "far" },
    ],
  },
  {
    id: "S6",
    scope: "supplementary",
    title: "Mobility / handover",
    goal: "Measure whether motion or handover produces a usable or disruptive session gap.",
    minimumData: ["speed", "handover", "latency spike", "session gap", "notes"],
    prompts: ["Is mobility-induced latency still inside the service envelope you want to discuss?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
    ],
    editableFields: [
      { key: "speed", label: "Speed", placeholder: "15 mph" },
      { key: "handover", label: "Handover?", placeholder: "yes / no" },
      { key: "latencySpike", label: "Latency Spike", placeholder: "31 ms" },
      { key: "sessionGap", label: "Session Gap", placeholder: "0 ms" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Single-site motion path and handover conditions." },
    ],
    completionFields: ["speed", "handover", "latencySpike", "sessionGap"],
    conditions: [
      { conditionId: "mobility-slow" },
      { conditionId: "mobility-fast" },
    ],
  },
  {
    id: "S7",
    scope: "supplementary",
    title: "Additional queue-discipline ablations",
    goal: "Compare queueing policies under the same service mix and load.",
    minimumData: ["queue mode", "p95", "p99", "drop rate", "notes"],
    prompts: ["Which queueing policy produces the best tail without starving lower-priority traffic?"],
    staticColumns: [
      { key: "conditionId", label: "Condition ID" },
      { key: "queueMode", label: "Queue Mode" },
    ],
    editableFields: [
      { key: "p95Ms", label: "p95 ms", placeholder: "29.8" },
      { key: "p99Ms", label: "p99 ms", placeholder: "37.5" },
      { key: "dropRate", label: "Drop Rate", placeholder: "0.5%" },
      { key: "notes", label: "Notes", type: "textarea", placeholder: "Service mix, scheduler config, starvation notes." },
    ],
    completionFields: ["p95Ms", "p99Ms", "dropRate"],
    conditions: [
      { conditionId: "queue-fifo", queueMode: "FIFO" },
      { conditionId: "queue-strict-priority", queueMode: "strict priority" },
      { conditionId: "queue-wfq", queueMode: "WFQ" },
    ],
  },
];

let state = loadState();
let saveUnavailable = false;

function createEmptyRunLogRow() {
  const row = {};
  RUN_LOG_COLUMNS.forEach((column) => {
    row[column.key] = "";
  });
  return row;
}

function emptyFieldsFor(definition) {
  const fields = {};
  definition.editableFields.forEach((field) => {
    fields[field.key] = "";
  });
  return fields;
}

function createDefaultState() {
  const summary = Object.fromEntries(CAMPAIGN_FIELDS.map((field) => [field.key, ""]));
  const serviceClasses = {};
  SERVICE_CLASS_DEFS.forEach((definition) => {
    serviceClasses[definition.id] = {
      focused: true,
      implementationPath: definition.implementationPath,
      notes: "",
    };
  });

  const figures = {};
  FIGURE_DEFS.forEach((definition) => {
    figures[definition.id] = {
      status: "not-started",
      note: "",
    };
  });

  const checklists = {};
  CHECKLIST_GROUPS.forEach((group) => {
    checklists[group.id] = Object.fromEntries(group.items.map((item) => [item.id, false]));
  });

  const experiments = {};
  EXPERIMENT_DEFS.forEach((definition) => {
    experiments[definition.id] = {
      takeawayNotes: "",
      artifactNotes: "",
      rows: Object.fromEntries(
        definition.conditions.map((condition) => [condition.conditionId, emptyFieldsFor(definition)])
      ),
    };
  });

  const readiness = Object.fromEntries(READINESS_ITEMS.map((item) => [item.id, false]));

  return {
    version: 1,
    updatedAt: "",
    summary,
    serviceClasses,
    figures,
    checklists,
    experiments,
    runLog: [createEmptyRunLogRow()],
    readiness,
    finalClaimNotes: "",
  };
}

function loadState() {
  const defaults = createDefaultState();

  try {
    const raw = window.localStorage.getItem(STORAGE_KEY);
    if (!raw) {
      return defaults;
    }
    const parsed = JSON.parse(raw);
    return hydrateState(parsed);
  } catch (error) {
    console.warn("Unable to load experiment tracker state.", error);
    return defaults;
  }
}

function hydrateState(raw) {
  const stateValue = createDefaultState();
  if (!raw || typeof raw !== "object") {
    return stateValue;
  }

  if (typeof raw.updatedAt === "string") {
    stateValue.updatedAt = raw.updatedAt;
  }

  if (raw.summary && typeof raw.summary === "object") {
    CAMPAIGN_FIELDS.forEach((field) => {
      if (typeof raw.summary[field.key] === "string") {
        stateValue.summary[field.key] = raw.summary[field.key];
      }
    });
  }

  if (raw.serviceClasses && typeof raw.serviceClasses === "object") {
    SERVICE_CLASS_DEFS.forEach((definition) => {
      const source = raw.serviceClasses[definition.id];
      if (!source || typeof source !== "object") {
        return;
      }
      if (typeof source.focused === "boolean") {
        stateValue.serviceClasses[definition.id].focused = source.focused;
      }
      if (typeof source.implementationPath === "string") {
        stateValue.serviceClasses[definition.id].implementationPath = source.implementationPath;
      }
      if (typeof source.notes === "string") {
        stateValue.serviceClasses[definition.id].notes = source.notes;
      }
    });
  }

  if (raw.figures && typeof raw.figures === "object") {
    FIGURE_DEFS.forEach((definition) => {
      const source = raw.figures[definition.id];
      if (!source || typeof source !== "object") {
        return;
      }
      if (typeof source.status === "string") {
        stateValue.figures[definition.id].status = source.status;
      }
      if (typeof source.note === "string") {
        stateValue.figures[definition.id].note = source.note;
      }
    });
  }

  if (raw.checklists && typeof raw.checklists === "object") {
    CHECKLIST_GROUPS.forEach((group) => {
      const source = raw.checklists[group.id];
      if (!source || typeof source !== "object") {
        return;
      }
      group.items.forEach((item) => {
        if (typeof source[item.id] === "boolean") {
          stateValue.checklists[group.id][item.id] = source[item.id];
        }
      });
    });
  }

  if (raw.experiments && typeof raw.experiments === "object") {
    EXPERIMENT_DEFS.forEach((definition) => {
      const source = raw.experiments[definition.id];
      if (!source || typeof source !== "object") {
        return;
      }
      if (typeof source.takeawayNotes === "string") {
        stateValue.experiments[definition.id].takeawayNotes = source.takeawayNotes;
      }
      if (typeof source.artifactNotes === "string") {
        stateValue.experiments[definition.id].artifactNotes = source.artifactNotes;
      }
      if (source.rows && typeof source.rows === "object") {
        definition.conditions.forEach((condition) => {
          const row = source.rows[condition.conditionId];
          if (!row || typeof row !== "object") {
            return;
          }
          definition.editableFields.forEach((field) => {
            if (typeof row[field.key] === "string") {
              stateValue.experiments[definition.id].rows[condition.conditionId][field.key] = row[field.key];
            }
          });
        });
      }
    });
  }

  if (Array.isArray(raw.runLog) && raw.runLog.length > 0) {
    stateValue.runLog = raw.runLog.map((row) => {
      const hydratedRow = createEmptyRunLogRow();
      if (row && typeof row === "object") {
        RUN_LOG_COLUMNS.forEach((column) => {
          if (typeof row[column.key] === "string") {
            hydratedRow[column.key] = row[column.key];
          }
        });
      }
      return hydratedRow;
    });
  }

  if (raw.readiness && typeof raw.readiness === "object") {
    READINESS_ITEMS.forEach((item) => {
      if (typeof raw.readiness[item.id] === "boolean") {
        stateValue.readiness[item.id] = raw.readiness[item.id];
      }
    });
  }

  if (typeof raw.finalClaimNotes === "string") {
    stateValue.finalClaimNotes = raw.finalClaimNotes;
  }

  return stateValue;
}

function persistState() {
  state.updatedAt = new Date().toISOString();
  try {
    window.localStorage.setItem(STORAGE_KEY, JSON.stringify(state));
  } catch (error) {
    saveUnavailable = true;
    console.warn("Unable to persist experiment tracker state.", error);
  }
  refreshDerivedUi();
}

function renderApp() {
  renderSidebarNav();
  renderSummaryForm();
  renderServiceClasses();
  renderFigures();
  renderChecklistGroups();
  renderExperiments();
  renderRunLog();
  renderReadiness();
  document.getElementById("final-claim-notes").value = state.finalClaimNotes;
  refreshDerivedUi();
}

function renderSidebarNav() {
  const navItems = [
    { id: "campaign-summary-section", label: "Campaign Summary", chip: "summary" },
    { id: "service-classes-section", label: "Service Classes", chip: "scope" },
    { id: "figures-section", label: "Figures", chip: `${countFigureComplete()}/${FIGURE_DEFS.length}` },
    { id: "checklists-section", label: "Checklists", chip: "plan" },
    ...EXPERIMENT_DEFS.map((definition) => ({
      id: `experiment-${definition.id}`,
      label: `${definition.id} ${definition.title}`,
      chip: definition.scope === "core" ? "core" : "supp",
    })),
    { id: "run-log-section", label: "Run Log", chip: `${countRunLogEntries()}` },
    { id: "readiness-section", label: "Readiness", chip: `${countCheckedItems(state.readiness)}/${READINESS_ITEMS.length}` },
    { id: "claim-notes-section", label: "Claim Notes", chip: "draft" },
  ];

  document.getElementById("sidebar-nav").innerHTML = navItems
    .map(
      (item) => `
        <a class="nav-link" href="#${escapeHtml(item.id)}">
          <span class="nav-label">${escapeHtml(item.label)}</span>
          <span class="nav-chip">${escapeHtml(item.chip)}</span>
        </a>
      `
    )
    .join("");
}

function renderSummaryForm() {
  const html = CAMPAIGN_FIELDS.map((field) => {
    const value = state.summary[field.key] || "";
    return `
      <div class="field-block">
        <label for="summary-${escapeHtml(field.key)}">${escapeHtml(field.label)}</label>
        <input
          id="summary-${escapeHtml(field.key)}"
          type="text"
          data-summary-key="${escapeHtml(field.key)}"
          value="${escapeHtml(value)}"
          placeholder="${escapeHtml(field.placeholder)}"
        >
      </div>
    `;
  }).join("");

  document.getElementById("campaign-summary-form").innerHTML = html;
}

function renderServiceClasses() {
  const html = SERVICE_CLASS_DEFS.map((definition) => {
    const serviceState = state.serviceClasses[definition.id];
    return `
      <article class="service-card">
        <div>
          <p class="eyebrow">Service Class</p>
          <h4>${escapeHtml(definition.title)}</h4>
        </div>
        <label class="checkbox-line">
          <input
            type="checkbox"
            data-service-id="${escapeHtml(definition.id)}"
            data-service-field="focused"
            ${serviceState.focused ? "checked" : ""}
          >
          Keep this service class in the main-paper scope
        </label>
        <div>
          <label class="form-label" for="service-path-${escapeHtml(definition.id)}">Current implementation path</label>
          <textarea
            id="service-path-${escapeHtml(definition.id)}"
            data-service-id="${escapeHtml(definition.id)}"
            data-service-field="implementationPath"
          >${escapeHtml(serviceState.implementationPath)}</textarea>
        </div>
        <div>
          <label class="form-label" for="service-notes-${escapeHtml(definition.id)}">Notes</label>
          <textarea
            id="service-notes-${escapeHtml(definition.id)}"
            data-service-id="${escapeHtml(definition.id)}"
            data-service-field="notes"
            placeholder="What is implemented, what is missing, what data path you trust."
          >${escapeHtml(serviceState.notes)}</textarea>
        </div>
      </article>
    `;
  }).join("");

  document.getElementById("service-classes").innerHTML = html;
}

function renderFigures() {
  const html = FIGURE_DEFS.map((definition) => {
    const figureState = state.figures[definition.id];
    return `
      <article class="figure-card">
        <div>
          <p class="eyebrow">${escapeHtml(definition.title)}</p>
          <h4>${escapeHtml(definition.headline)}</h4>
        </div>
        <p>${escapeHtml(definition.description)}</p>
        <div>
          <label class="form-label" for="figure-status-${escapeHtml(definition.id)}">Status</label>
          <select id="figure-status-${escapeHtml(definition.id)}" data-figure-id="${escapeHtml(definition.id)}" data-figure-field="status">
            ${renderOptions(["not-started", "in-progress", "complete"], figureState.status)}
          </select>
        </div>
        <div>
          <label class="form-label" for="figure-note-${escapeHtml(definition.id)}">Result summary</label>
          <textarea
            id="figure-note-${escapeHtml(definition.id)}"
            data-figure-id="${escapeHtml(definition.id)}"
            data-figure-field="note"
            placeholder="What this figure will show once the data is ready."
          >${escapeHtml(figureState.note)}</textarea>
        </div>
      </article>
    `;
  }).join("");

  document.getElementById("main-figures").innerHTML = html;
}

function renderChecklistGroups() {
  const html = CHECKLIST_GROUPS.map((group) => `
    <article class="checklist-card">
      <div>
        <p class="eyebrow">Checklist</p>
        <h4>${escapeHtml(group.title)}</h4>
      </div>
      <p class="support-note">${escapeHtml(group.description)}</p>
      <ul>
        ${group.items
          .map((item) => `
            <li>
              <label>
                <div class="checkbox-line">
                  <input
                    type="checkbox"
                    data-checklist-group="${escapeHtml(group.id)}"
                    data-checklist-item="${escapeHtml(item.id)}"
                    ${state.checklists[group.id][item.id] ? "checked" : ""}
                  >
                  <span>${escapeHtml(item.label)}</span>
                </div>
                ${group.id === "claims" ? "<small>Leave unchecked until the evidence is strong enough to support the claim safely.</small>" : ""}
              </label>
            </li>
          `)
          .join("")}
      </ul>
    </article>
  `).join("");

  document.getElementById("checklist-groups").innerHTML = html;
}

function renderExperiments() {
  const container = document.getElementById("experiments-container");
  const sections = [];

  ["core", "supplementary"].forEach((scope) => {
    sections.push(`<p class="scope-heading">${scope === "core" ? "Core Experiments" : "Supplementary Experiments"}</p>`);
    EXPERIMENT_DEFS.filter((definition) => definition.scope === scope).forEach((definition) => {
      const experimentState = state.experiments[definition.id];
      const completion = getExperimentCompletion(definition);
      const statusClass = completion.complete ? "ready" : completion.completeRows > 0 ? "partial" : "";
      const staticHeaders = definition.staticColumns
        .map((column) => `<th>${escapeHtml(column.label)}</th>`)
        .join("");
      const editableHeaders = definition.editableFields
        .map((field) => `<th>${escapeHtml(field.label)}</th>`)
        .join("");
      const rows = definition.conditions
        .map((condition) => renderExperimentRow(definition, condition, experimentState.rows[condition.conditionId]))
        .join("");

      sections.push(`
        <details class="experiment-card" id="experiment-${escapeHtml(definition.id)}" ${scope === "core" ? "open" : ""}>
          <summary>
            <div>
              <div class="experiment-title-row">
                <span class="scope-pill ${escapeHtml(scope)}">${scope === "core" ? "Main paper" : "Supplement"}</span>
                <h4 class="experiment-title">${escapeHtml(definition.id)} ${escapeHtml(definition.title)}</h4>
                <span class="status-pill ${statusClass}" data-experiment-status="${escapeHtml(definition.id)}">
                  ${escapeHtml(statusLabelFor(completion))}
                </span>
              </div>
              <p class="experiment-summary-copy">${escapeHtml(definition.goal)}</p>
            </div>
            <div class="experiment-progress">
              <div class="progress-label">
                <span>Row completion</span>
                <span data-experiment-progress-text="${escapeHtml(definition.id)}">${escapeHtml(progressTextFor(completion))}</span>
              </div>
              <div class="progress-track">
                <span class="progress-fill" data-experiment-progress-fill="${escapeHtml(definition.id)}" style="width: ${completion.percent}%"></span>
              </div>
            </div>
          </summary>
          <div class="experiment-body">
            <div class="experiment-info-card">
              <p class="eyebrow">Minimum Data</p>
              <div class="chip-list">
                ${definition.minimumData.map((entry) => `<span class="mini-chip">${escapeHtml(entry)}</span>`).join("")}
              </div>
            </div>

            <div class="table-scroll">
              <table>
                <thead>
                  <tr>
                    ${staticHeaders}
                    ${editableHeaders}
                  </tr>
                </thead>
                <tbody>
                  ${rows}
                </tbody>
              </table>
            </div>

            <div class="notes-grid">
              <div class="notes-panel">
                <p class="eyebrow">Figure Takeaway</p>
                <h4>Interpretation notes</h4>
                <ul class="prompt-list">
                  ${definition.prompts.map((prompt) => `<li>${escapeHtml(prompt)}</li>`).join("")}
                </ul>
                <textarea
                  data-experiment-notes="${escapeHtml(definition.id)}"
                  data-experiment-notes-field="takeawayNotes"
                  placeholder="What does this experiment say about the operating envelope?"
                >${escapeHtml(experimentState.takeawayNotes)}</textarea>
              </div>
              <div class="notes-panel">
                <p class="eyebrow">Artifacts</p>
                <h4>Commands and log notes</h4>
                <p>Record log paths, scripts used, broker settings, emulator settings, or anything you will want later when writing the methods section.</p>
                <textarea
                  data-experiment-notes="${escapeHtml(definition.id)}"
                  data-experiment-notes-field="artifactNotes"
                  placeholder="Commands, configs, log bundles, or manual notes."
                >${escapeHtml(experimentState.artifactNotes)}</textarea>
              </div>
            </div>
          </div>
        </details>
      `);
    });
  });

  container.innerHTML = sections.join("");
}

function renderExperimentRow(definition, condition, rowState) {
  const staticCells = definition.staticColumns.map((column) => {
    const value = condition[column.key] || "";
    const content = column.key === "conditionId" ? `<code>${escapeHtml(value)}</code>` : escapeHtml(value);
    return `<td class="static-cell">${content}</td>`;
  }).join("");

  const editableCells = definition.editableFields.map((field) => {
    const value = rowState[field.key] || "";
    if (field.type === "textarea") {
      return `
        <td>
          <textarea
            class="table-textarea"
            data-experiment-id="${escapeHtml(definition.id)}"
            data-condition-id="${escapeHtml(condition.conditionId)}"
            data-field="${escapeHtml(field.key)}"
            placeholder="${escapeHtml(field.placeholder || "")}"
          >${escapeHtml(value)}</textarea>
        </td>
      `;
    }

    return `
      <td>
        <input
          class="table-input"
          type="text"
          inputmode="${escapeHtml(field.inputMode || "text")}"
          data-experiment-id="${escapeHtml(definition.id)}"
          data-condition-id="${escapeHtml(condition.conditionId)}"
          data-field="${escapeHtml(field.key)}"
          value="${escapeHtml(value)}"
          placeholder="${escapeHtml(field.placeholder || "")}"
        >
      </td>
    `;
  }).join("");

  return `<tr>${staticCells}${editableCells}</tr>`;
}

function renderRunLog() {
  const headerCells = RUN_LOG_COLUMNS.map((column) => `<th>${escapeHtml(column.label)}</th>`).join("");
  const rows = state.runLog.map((row, index) => `
    <tr>
      ${RUN_LOG_COLUMNS.map((column) => `<td>${renderRunLogField(column, row[column.key], index)}</td>`).join("")}
      <td class="run-log-actions">
        <button class="row-delete" data-delete-run-log="${index}" ${state.runLog.length === 1 ? "disabled" : ""}>Delete</button>
      </td>
    </tr>
  `).join("");

  document.getElementById("run-log-table").innerHTML = `
    <div class="table-scroll">
      <table>
        <thead>
          <tr>
            ${headerCells}
            <th>Actions</th>
          </tr>
        </thead>
        <tbody>${rows}</tbody>
      </table>
    </div>
  `;
}

function renderRunLogField(column, value, index) {
  if (column.type === "textarea") {
    return `
      <textarea
        class="table-textarea"
        data-run-index="${index}"
        data-run-field="${escapeHtml(column.key)}"
        placeholder="${escapeHtml(column.label)}"
      >${escapeHtml(value || "")}</textarea>
    `;
  }

  if (column.type === "select") {
    return `
      <select
        class="table-select"
        data-run-index="${index}"
        data-run-field="${escapeHtml(column.key)}"
      >
        ${renderOptions(column.options, value || "")}
      </select>
    `;
  }

  const inputType = column.type === "date" ? "date" : "text";
  return `
    <input
      class="table-input"
      type="${inputType}"
      data-run-index="${index}"
      data-run-field="${escapeHtml(column.key)}"
      value="${escapeHtml(value || "")}"
      placeholder="${escapeHtml(column.label)}"
    >
  `;
}

function renderReadiness() {
  document.getElementById("readiness-checklist").innerHTML = READINESS_ITEMS.map((item) => `
    <article class="checklist-card">
      <label class="checkbox-line">
        <input
          type="checkbox"
          data-readiness-item="${escapeHtml(item.id)}"
          ${state.readiness[item.id] ? "checked" : ""}
        >
        <span>${escapeHtml(item.label)}</span>
      </label>
    </article>
  `).join("");
}

function refreshDerivedUi() {
  document.getElementById("sidebar-stats").innerHTML = renderSidebarStats();
  renderSidebarNav();
  refreshSaveStatus();
  refreshExperimentProgressBadges();
}

function renderSidebarStats() {
  const cards = [
    { label: "Core done", value: `${countCheckedItems(state.checklists.core)}/${CHECKLIST_GROUPS[0].items.length}` },
    { label: "Supplement", value: `${countCheckedItems(state.checklists.supplementary)}/${CHECKLIST_GROUPS[1].items.length}` },
    { label: "Figures", value: `${countFigureComplete()}/${FIGURE_DEFS.length}` },
    { label: "Rows with data", value: `${countRowsWithData()}` },
    { label: "Run logs", value: `${countRunLogEntries()}` },
    { label: "Readiness", value: `${countCheckedItems(state.readiness)}/${READINESS_ITEMS.length}` },
  ];

  return cards
    .map(
      (card) => `
        <div class="stat-card">
          <span class="stat-value">${escapeHtml(card.value)}</span>
          <span class="stat-label">${escapeHtml(card.label)}</span>
        </div>
      `
    )
    .join("");
}

function refreshSaveStatus() {
  const chip = document.getElementById("save-status");
  if (saveUnavailable) {
    chip.textContent = "Autosave unavailable";
    return;
  }

  if (!state.updatedAt) {
    chip.textContent = "Using default workspace";
    return;
  }

  const time = new Date(state.updatedAt);
  chip.textContent = `Saved ${time.toLocaleString()}`;
}

function refreshExperimentProgressBadges() {
  EXPERIMENT_DEFS.forEach((definition) => {
    const completion = getExperimentCompletion(definition);
    const statusElement = document.querySelector(`[data-experiment-status="${definition.id}"]`);
    const textElement = document.querySelector(`[data-experiment-progress-text="${definition.id}"]`);
    const fillElement = document.querySelector(`[data-experiment-progress-fill="${definition.id}"]`);
    if (!statusElement || !textElement || !fillElement) {
      return;
    }
    statusElement.textContent = statusLabelFor(completion);
    statusElement.className = `status-pill ${completion.complete ? "ready" : completion.completeRows > 0 ? "partial" : ""}`.trim();
    textElement.textContent = progressTextFor(completion);
    fillElement.style.width = `${completion.percent}%`;
  });
}

function getExperimentCompletion(definition) {
  const experimentState = state.experiments[definition.id];
  let completeRows = 0;
  definition.conditions.forEach((condition) => {
    const row = experimentState.rows[condition.conditionId];
    const rowReady = definition.completionFields.every((field) => hasContent(row[field]));
    if (rowReady) {
      completeRows += 1;
    }
  });
  const totalRows = definition.conditions.length;
  const percent = totalRows === 0 ? 0 : Math.round((completeRows / totalRows) * 100);
  return {
    completeRows,
    totalRows,
    percent,
    complete: totalRows > 0 && completeRows === totalRows,
  };
}

function progressTextFor(completion) {
  return `${completion.completeRows}/${completion.totalRows} rows ready`;
}

function statusLabelFor(completion) {
  if (completion.complete) {
    return "Ready";
  }
  if (completion.completeRows > 0) {
    return "In progress";
  }
  return "No data";
}

function countFigureComplete() {
  return Object.values(state.figures).filter((figure) => figure.status === "complete").length;
}

function countCheckedItems(map) {
  return Object.values(map).filter(Boolean).length;
}

function countRowsWithData() {
  let count = 0;
  EXPERIMENT_DEFS.forEach((definition) => {
    definition.conditions.forEach((condition) => {
      const row = state.experiments[definition.id].rows[condition.conditionId];
      if (Object.values(row).some((value) => hasContent(value))) {
        count += 1;
      }
    });
  });
  return count;
}

function countRunLogEntries() {
  return state.runLog.filter((row) =>
    RUN_LOG_COLUMNS.some((column) => hasContent(row[column.key]))
  ).length;
}

function hasContent(value) {
  return typeof value === "string" && value.trim().length > 0;
}

function renderOptions(options, selectedValue) {
  return options
    .map((option) => `<option value="${escapeHtml(option)}" ${option === selectedValue ? "selected" : ""}>${escapeHtml(option || "Select")}</option>`)
    .join("");
}

function escapeHtml(value) {
  return String(value ?? "")
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#39;");
}

function normalizeMarkdownCell(value) {
  return String(value ?? "").replaceAll("\n", "<br>").replaceAll("|", "\\|");
}

function buildMarkdownTable(headers, rows) {
  const headerLine = `| ${headers.join(" | ")} |`;
  const dividerLine = `| ${headers.map(() => "---").join(" | ")} |`;
  const bodyLines = rows.length
    ? rows.map((row) => `| ${row.map((cell) => normalizeMarkdownCell(cell)).join(" | ")} |`)
    : [`| ${headers.map(() => "").join(" | ")} |`];
  return [headerLine, dividerLine, ...bodyLines].join("\n");
}

function downloadText(filename, text, type) {
  const blob = new Blob([text], { type });
  const url = URL.createObjectURL(blob);
  const anchor = document.createElement("a");
  anchor.href = url;
  anchor.download = filename;
  anchor.click();
  URL.revokeObjectURL(url);
}

function exportJson() {
  downloadText("edge4av-experiment-tracker.json", JSON.stringify(state, null, 2), "application/json");
}

function exportMarkdown() {
  const sections = [];
  sections.push("# Edge4AV Experiment Tracker Export");
  sections.push("");
  sections.push(`Last updated: ${state.updatedAt || "Not saved yet"}`);
  sections.push("");
  sections.push("## Campaign Summary");
  sections.push("");
  CAMPAIGN_FIELDS.forEach((field) => {
    sections.push(`- ${field.label}: ${state.summary[field.key] || ""}`);
  });
  sections.push("");
  sections.push("## Main-Paper Service Classes");
  sections.push("");
  SERVICE_CLASS_DEFS.forEach((definition) => {
    const serviceState = state.serviceClasses[definition.id];
    sections.push(`### ${definition.title}`);
    sections.push("");
    sections.push(`- In main scope: ${serviceState.focused ? "yes" : "no"}`);
    sections.push(`- Current implementation path: ${serviceState.implementationPath || ""}`);
    sections.push(`- Notes: ${serviceState.notes || ""}`);
    sections.push("");
  });
  sections.push("## Main-Paper Figures to Earn");
  sections.push("");
  FIGURE_DEFS.forEach((definition) => {
    const figureState = state.figures[definition.id];
    sections.push(`- ${definition.title}: ${definition.headline} (${figureState.status})`);
    sections.push(`  ${figureState.note || ""}`);
  });
  sections.push("");
  sections.push("## Experiment Checklist");
  sections.push("");
  CHECKLIST_GROUPS.forEach((group) => {
    sections.push(`### ${group.title}`);
    sections.push("");
    group.items.forEach((item) => {
      sections.push(`- [${state.checklists[group.id][item.id] ? "x" : " "}] ${item.label}`);
    });
    sections.push("");
  });

  EXPERIMENT_DEFS.forEach((definition) => {
    const experimentState = state.experiments[definition.id];
    sections.push(`## ${definition.id} ${definition.title}`);
    sections.push("");
    sections.push("### Goal");
    sections.push("");
    sections.push(definition.goal);
    sections.push("");
    sections.push("### Minimum Data to Collect");
    sections.push("");
    definition.minimumData.forEach((entry) => {
      sections.push(`- ${entry}`);
    });
    sections.push("");
    sections.push("### Results");
    sections.push("");

    const headers = [
      ...definition.staticColumns.map((column) => column.label),
      ...definition.editableFields.map((field) => field.label),
    ];
    const rows = definition.conditions.map((condition) => {
      const resultRow = experimentState.rows[condition.conditionId];
      return [
        ...definition.staticColumns.map((column) => condition[column.key] || ""),
        ...definition.editableFields.map((field) => resultRow[field.key] || ""),
      ];
    });
    sections.push(buildMarkdownTable(headers, rows));
    sections.push("");
    sections.push("### Figure Takeaway");
    sections.push("");
    sections.push(experimentState.takeawayNotes || "");
    sections.push("");
    sections.push("### Artifacts / Command Notes");
    sections.push("");
    sections.push(experimentState.artifactNotes || "");
    sections.push("");
  });

  sections.push("## Run Log Index");
  sections.push("");
  sections.push(buildMarkdownTable(
    RUN_LOG_COLUMNS.map((column) => column.label),
    state.runLog.map((row) => RUN_LOG_COLUMNS.map((column) => row[column.key] || ""))
  ));
  sections.push("");
  sections.push("## Paper Readiness Check");
  sections.push("");
  READINESS_ITEMS.forEach((item) => {
    sections.push(`- [${state.readiness[item.id] ? "x" : " "}] ${item.label}`);
  });
  sections.push("");
  sections.push("## Final Claim Notes");
  sections.push("");
  sections.push(state.finalClaimNotes || "");
  sections.push("");

  downloadText("edge4av-experiment-export.md", sections.join("\n"), "text/markdown");
}

function importJson(file) {
  if (!file) {
    return;
  }

  const reader = new FileReader();
  reader.onload = () => {
    try {
      const parsed = JSON.parse(String(reader.result));
      state = hydrateState(parsed);
      persistState();
      renderApp();
    } catch (error) {
      window.alert("That file is not valid tracker JSON.");
    }
  };
  reader.readAsText(file);
}

function resetState() {
  if (!window.confirm("Reset the local experiment tracker data for this browser?")) {
    return;
  }
  state = createDefaultState();
  persistState();
  renderApp();
}

function handleMutableInput(event) {
  const target = event.target;

  if (!(target instanceof HTMLElement)) {
    return;
  }

  if (target.dataset.summaryKey) {
    state.summary[target.dataset.summaryKey] = target.value;
    persistState();
    return;
  }

  if (target.dataset.serviceId && target.dataset.serviceField) {
    state.serviceClasses[target.dataset.serviceId][target.dataset.serviceField] =
      target instanceof HTMLInputElement && target.type === "checkbox" ? target.checked : target.value;
    persistState();
    return;
  }

  if (target.dataset.figureId && target.dataset.figureField) {
    state.figures[target.dataset.figureId][target.dataset.figureField] = target.value;
    persistState();
    return;
  }

  if (target.dataset.checklistGroup && target.dataset.checklistItem) {
    state.checklists[target.dataset.checklistGroup][target.dataset.checklistItem] =
      target instanceof HTMLInputElement ? target.checked : Boolean(target.value);
    persistState();
    return;
  }

  if (target.dataset.experimentId && target.dataset.conditionId && target.dataset.field) {
    state.experiments[target.dataset.experimentId].rows[target.dataset.conditionId][target.dataset.field] = target.value;
    persistState();
    return;
  }

  if (target.dataset.experimentNotes && target.dataset.experimentNotesField) {
    state.experiments[target.dataset.experimentNotes][target.dataset.experimentNotesField] = target.value;
    persistState();
    return;
  }

  if (target.dataset.runIndex && target.dataset.runField) {
    const runIndex = Number(target.dataset.runIndex);
    if (!Number.isNaN(runIndex) && state.runLog[runIndex]) {
      state.runLog[runIndex][target.dataset.runField] = target.value;
      persistState();
    }
    return;
  }

  if (target.dataset.readinessItem) {
    state.readiness[target.dataset.readinessItem] =
      target instanceof HTMLInputElement ? target.checked : Boolean(target.value);
    persistState();
    return;
  }

  if (target.id === "final-claim-notes") {
    state.finalClaimNotes = target.value;
    persistState();
  }
}

document.addEventListener("input", handleMutableInput);
document.addEventListener("change", handleMutableInput);

document.addEventListener("click", (event) => {
  const target = event.target;

  if (!(target instanceof HTMLElement)) {
    return;
  }

  if (target.id === "export-json") {
    exportJson();
    return;
  }

  if (target.id === "export-md") {
    exportMarkdown();
    return;
  }

  if (target.id === "import-json") {
    document.getElementById("import-file").click();
    return;
  }

  if (target.id === "reset-state") {
    resetState();
    return;
  }

  if (target.id === "add-run-log-row") {
    state.runLog.push(createEmptyRunLogRow());
    persistState();
    renderRunLog();
    return;
  }

  if (target.dataset.deleteRunLog) {
    const rowIndex = Number(target.dataset.deleteRunLog);
    if (!Number.isNaN(rowIndex) && state.runLog.length > 1) {
      state.runLog.splice(rowIndex, 1);
      persistState();
      renderRunLog();
    }
  }
});

document.getElementById("sidebar-nav").addEventListener("click", (event) => {
  const target = event.target instanceof HTMLElement ? event.target.closest("a[href^='#']") : null;
  if (!(target instanceof HTMLAnchorElement)) {
    return;
  }

  const id = target.getAttribute("href")?.slice(1);
  if (!id) {
    return;
  }

  const destination = document.getElementById(id);
  if (destination instanceof HTMLDetailsElement) {
    destination.open = true;
  }
});

document.getElementById("import-file").addEventListener("change", (event) => {
  const target = event.target;
  if (!(target instanceof HTMLInputElement)) {
    return;
  }
  importJson(target.files?.[0] || null);
  target.value = "";
});

renderApp();
