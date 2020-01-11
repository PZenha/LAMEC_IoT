import React, { Component } from 'react';
import '../App1.css'
import CanvasJSReact from '../assets/canvasjs.react';
var CanvasJSChart = CanvasJSReact.CanvasJSChart;

 
var dataPoints =[];
var dataPoints_1 = [];
var dataPoints_2 = [];


class LineChart extends Component {
	constructor(props){
		super(props);
		this.state={
			results: ""
		};
		this.getData = this.getData.bind(this);
	}
	async postData() {
		try{
			let result = await fetch('http://localhost:4000/actuator',{
				method: 'POST',
				//mode: 'no-cors',
				headers: {
					'Accept': 'application/json',
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					Relay: 'ON'
				})
			});
			console.log(result)
			this.setState({results: new Date().toLocaleString()});
		} catch(e){
			console.log(e)
		}
	}
	
	async postData_1() {
		try{
			let result = await fetch('http://localhost:4000/actuator',{
				method: 'POST',
				//mode: 'no-cors',
				headers: {
					'Accept': 'application/json',
					'Content-Type': 'application/json',
				},
				body: JSON.stringify({
					Relay: 'OFF'
				})
			});
			console.log(result)
		} catch(e){
			console.log(e)
		}
	}

	getData(){
		var chart = this.chart;
		var chart1 = this.chart1;
		var chart2 = this.chart2;
		fetch('http://localhost:4000/espdata/')
		.then((response) => {
			return response.json();
		})
			//return response.json();
		
		.then((ResData) => {
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
			this.setState({results: new Date(ResData.LastTime).toLocaleString()}, () => {
				console.log(this.state.results);
			});
			console.log(this.results);
			console.log(ResData);
		});
	}

	DataUpdate(){
		var chart = this.chart;
		var chart1 = this.chart1;
		var chart2 = this.chart2;
		fetch('http://localhost:4000/espdata/')
		.then((response) => {
			return response.json();
		})
		.then((ResData) => {
			//var newDate = new Date(ResData.dados[0].Time).getTime();
			var newDate = new Date(ResData.dados[0].Time).getTime();
			var oldDate = new Date(dataPoints[0].x).getTime();
			if(newDate > oldDate){
				while(dataPoints.length > 0){
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
			this.setState({results: new Date(ResData.LastTime).toLocaleString()}, () => {
				console.log(this.state.results);
			});
		})

	}
	componentDidMount(){
		this.getData();
		setInterval(() => this.DataUpdate(),5000) 
	}
	/*componentWillUnmount(){
		clearInterval(this.interval);
	} */

	render() {	
		const options = {
			theme: "white1",
			title: {
				text: "Temperatura do Ar"
			},
			axisY: {
				title: "Temperatura (ºC)",
				//prefix: "ºC",
				//sufix: "ºC",
				includeZero: false
			},
			data: [{
				type: "area",
				xValueFormatString: "HH:mm:ss",
				yValueFormatString: "##,##0.00 ºC",
				dataPoints: dataPoints
			}]
		}
		const options_1 = {
			theme: "white1",
			title: {
				text: "Humidade do Ar"
			},
			axisY: {
				title: "Humidade (%)",
				//prefix: "%",
				includeZero: false
			},
			data: [{
				type: "area",
				xValueFormatString: "HH:mm:ss",
				yValueFormatString: "##,#0 %",
				dataPoints: dataPoints_1
			}]
		}
		const options_2 = {
			theme: "white1",
			title: {
				text: "Humidade do Solo"
			},
			axisY: {
				title: "Humidade (%)",
				//prefix: "%",
				includeZero: false
			},
			data: [{
				type: "area",
				xValueFormatString: "HH:mm:ss",
				yValueFormatString: "##,#0 %",
				dataPoints: dataPoints_2
			}]
		}

		return (
		<div className="Global">
			<h1 className="Titulo">Plataforma de Controlo de Humidade e Temperatura</h1>
			Ativar motor: <button onClick={() => this.postData()}>ON</button><br></br>
			Desativar motor: <button onClick={() => this.postData_1()}>OFF</button>
		<h2>Motor foi ligado ás: {this.state.results}</h2>
			<div className="GraphsAr">
				<div className="TAr">
				<CanvasJSChart options = {options} 
					 onRef={ref => this.chart = ref}
				/>
				</div>
				
				<div className="HAr">
				<CanvasJSChart options = {options_1} 
					 onRef={ref => this.chart1 = ref}
				/>
				</div>
				
			</div>
			<br></br>
			<div className="GraphSolo">
			<CanvasJSChart options = {options_2} 
				 onRef={ref => this.chart2 = ref}
			/>
			</div>

			{/*You can get reference to the chart instance as shown above using onRef. This allows you to access all chart properties and methods*/}
		</div>
		);
	}
}
 
export default LineChart;