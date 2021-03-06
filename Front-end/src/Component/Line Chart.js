import React, { Component } from "react";
import "../App1.css";
import CanvasJSReact from "../assets/canvasjs.react";
import navBar from "./navbar";
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

var dataPoints = [];
var dataPoints_1 = [];
var dataPoints_2 = [];

class LineChart extends Component {
  constructor(props) {
    super(props);
    this.state = {
      results: ""
    };
    this.getData = this.getData.bind(this);
  }
  async postData() {
    try {
      let result = await fetch("http://localhost:4000/actuator", {
        method: "POST",
        //mode: 'no-cors',
        headers: {
          Accept: "application/json",
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          Relay: "ON"
        })
      });
      console.log(result);
      this.setState({ results: new Date().toLocaleString() });
    } catch (e) {
      console.log(e);
    }
  }

  async postData_1() {
    try {
      let result = await fetch("http://localhost:4000/actuator", {
        method: "POST",
        //mode: 'no-cors',
        headers: {
          Accept: "application/json",
          "Content-Type": "application/json"
        },
        body: JSON.stringify({
          Relay: "OFF"
        })
      });
      console.log(result);
    } catch (e) {
      console.log(e);
    }
  }

  getData() {
    var chart = this.chart;
    var chart1 = this.chart1;
    var chart2 = this.chart2;
    fetch("http://localhost:4000/espdata/")
      .then(response => {
        return response.json();
      })
      //return response.json();

      .then(ResData => {
        //console.log(ResData);
        //this.setState({results: ResData.data});
        //console.log(this.results)
        //console.log(this.results);
        for (var i = 0; i < 10; i++) {
          var today = new Date(ResData.dados[i].Time);
          dataPoints.push({
            x: today,
            y: ResData.dados[i].T_air
          });
          dataPoints_1.push({
            x: today,
            y: ResData.dados[i].H_air
          });
          dataPoints_2.push({
            x: today,
            y: ResData.dados[i].soil
          });
        }
        chart.render();
        chart1.render();
        chart2.render();
        this.setState({ results: ResData.LastTime.toLocaleString() }, () => {
          console.log(this.state.results);
        });
        // console.log(this.results);
        console.log(ResData);
      });
  }

  DataUpdate() {
    var chart = this.chart;
    var chart1 = this.chart1;
    var chart2 = this.chart2;
    fetch("http://localhost:4000/espdata/")
      .then(response => {
        return response.json();
      })
      .then(ResData => {
        //var newDate = new Date(ResData.dados[0].Time).getTime();
        var newDate = new Date(ResData.dados[0].Time).getTime();
        var oldDate = new Date(dataPoints[0].x).getTime();
        if (newDate > oldDate) {
          while (dataPoints.length > 0) {
            dataPoints.pop();
            dataPoints_1.pop();
            dataPoints_2.pop();
          }
          for (var i = 0; i < 10; i++) {
            var today = new Date(ResData.dados[i].Time);
            dataPoints.push({
              x: today,
              y: ResData.dados[i].T_air
            });
            dataPoints_1.push({
              x: today,
              y: ResData.dados[i].H_air
            });
            dataPoints_2.push({
              x: today,
              y: ResData.dados[i].soil
            });
          }
        }
        chart.render();
        chart1.render();
        chart2.render();
        this.setState({ results: ResData.LastTime.toLocaleString() }, () => {
          console.log(this.state.results);
        });
      });
  }
  componentDidMount() {
    this.getData();
    setInterval(() => this.DataUpdate(), 5000);
  }
  /*componentWillUnmount(){
		clearInterval(this.interval);
	} */

  render() {
    const options = {
      //theme: "white1",
      //backgroundColor: "  #e6ffe6",
      title: {
        text: "Temperatura do Ar"
      },
      axisY: {
        title: "Temperatura (ºC)",
        //prefix: "ºC",
        //sufix: "ºC",
        includeZero: false
      },
      data: [
        {
          type: "area",
          xValueFormatString: "HH:mm:ss",
          yValueFormatString: "##,##0.00 ºC",
          dataPoints: dataPoints
        }
      ]
    };
    const options_1 = {
      // backgroundColor: "  #e6ffe6",
      title: {
        text: "Humidade do Ar"
      },
      axisY: {
        title: "Humidade (%)",
        //prefix: "%",
        includeZero: false
      },
      data: [
        {
          type: "area",
          xValueFormatString: "HH:mm:ss",
          yValueFormatString: "##,#0 %",
          dataPoints: dataPoints_1
        }
      ]
    };
    const options_2 = {
      //backgroundColor: "  #e6ffe6",
      title: {
        text: "Humidade do Solo"
      },
      axisY: {
        title: "Humidade (%)",
        //prefix: "%",
        includeZero: false
      },
      data: [
        {
          type: "area",
          xValueFormatString: "HH:mm:ss",
          yValueFormatString: "##,#0 %",
          dataPoints: dataPoints_2
        }
      ]
    };

    return (
      <div className="Global">
        <div className="navbar">
          <div className="logo">
            <img src={require("./isep.png")} />
          </div>
          <div className="title">
            <h2>IoT - Plataforma de controlo de Humidade e Temperatura</h2>
          </div>
        </div>
        <div className="options">
          <div className="laston">
            <h3>Motor foi ligado ás:</h3>
            <h2>{this.state.results}</h2>
          </div>
          <div className="motor">
            <div className="mottext">
              <h2>Motor</h2>
            </div>
            <div className="motbutton">
              <button onClick={this.postData}>ON</button> &emsp;
              <button onClick={this.postData_1}>OFF</button>
            </div>
          </div>
        </div>
        <div className="chartWrapper">
          <div className="TAr">
            <CanvasJSChart
              options={options}
              onRef={ref => (this.chart = ref)}
            />
          </div>

          <div className="HAr">
            <CanvasJSChart
              options={options_1}
              onRef={ref => (this.chart1 = ref)}
            />
          </div>

          <div className="GraphSolo">
            <CanvasJSChart
              options={options_2}
              onRef={ref => (this.chart2 = ref)}
            />
          </div>
        </div>
        {/*You can get reference to the chart instance as shown above using onRef. This allows you to access all chart properties and methods*/}
      </div>
    );
  }
}

export default LineChart;
