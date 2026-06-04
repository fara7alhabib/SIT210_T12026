const express = require("express");
const mqtt = require("mqtt");
const path = require("path");

const app = express();
const port = 3000;

const client = mqtt.connect("mqtt://broker.hivemq.com");
const topic = "farah/linda/lights";

app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

client.on("connect", () => {
  console.log("Connected to MQTT broker");
});

app.post("/light", (req, res) => {
  const room = req.body.room;

  if (!["living room", "bathroom", "closet"].includes(room)) {
    return res.status(400).send("Invalid room");
  }

  client.publish(topic, room);
  console.log("Published:", room);

  res.send(`Command sent for ${room}`);
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});