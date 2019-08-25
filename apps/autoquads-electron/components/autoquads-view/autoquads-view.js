// Web Modules Imports
import { LitElement, html, css } from '../../web_modules/lit-element.js';
import '../../web_modules/@vaadin/vaadin-button.js';
import '../../web_modules/@vaadin/vaadin-split-layout.js';
import { PubSub } from '../../web_modules/pubsub-js.js';
import * as THREE from '../../web_modules/three.js';
import { connect } from '../../web_modules/pwa-helpers.js';

// Components Imports
import '../mesh-view/mesh-view.js';
import '../autoquads-side-bar/autoquads-side-bar.js';
import { MeshProvider } from '../mesh-provider/mesh-provider.js';
import { AutoquadsModelMeshProvider } from '../autoquads-model-mesh-provider/autoquads-model-mesh-provider.js';
import { AutoquadsSuopMeshProvider } from '../autoquads-suop-mesh-provider/autoquads-suop-mesh-provider.js';
import { store } from '../../redux/store.js';

export class AutoquadsView extends connect(store)(LitElement) {
    static get styles() {
        return [css`
            :host {
                display: flex;
                width: 100%;
                height: 100%;
            }

            .outer-split {
                flex: 1;
            }

            .inner-split {
                width: 100%;
                height: 100%;
            }

            /* https://stackoverflow.com/questions/41800823/hide-polymer-element */
            /* [hidden] {
                display: none;
            } */
        `];
    }

    render() {
        return html`  
            <vaadin-split-layout class="outer-split">
                <autoquads-side-bar></autoquads-side-bar>
                <vaadin-split-layout orientation="horizontal" class="inner-split">
                    <mesh-view 
                        id="model-mesh-view"
                        use-lights
                        enable-mesh-rotation
                        enable-vertex-selection
                        caption="Mesh View"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        show-wireframe="${this.showWireframe}"
                        background-color="${this.modelViewportColor}"
                        mesh-color="${this.modelColor}"
                        .meshProvider="${this.modelMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        show-grid-texture>
                    </mesh-view>
                    <mesh-view 
                        id="suop-mesh-view"
                        enable-face-dragging caption="Suop View"
                        show-grid="${this.showUnitGrid}"
                        grid-horizontal-color="${this.gridHorizontalColor}"
                        grid-vertical-color="${this.gridVerticalColor}"
                        grid-background-color1="${this.gridBackgroundColor1}"
                        grid-background-color2="${this.gridBackgroundColor2}"
                        grid-size="${this.gridSize}"
                        grid-texture-size="${this.gridTextureSize}"
                        grid-line-width="${this.gridLineWidth}"
                        show-wireframe="${this.showWireframe}"
                        background-color="${this.suopViewportColor}"
                        mesh-color="${this.solverColor}"
                        .meshProvider="${this.suopMeshProvider}"
                        mesh-interaction="${this.meshInteraction}"
                        highlighted-face-color="${this.highlightedFaceColor}"
                        dragged-face-color="${this.draggedFaceColor}"
                        selected-face-color="${this.fixedFaceColor}"
                        show-debug-data="${this.showOptimizationDataMonitor}"
                        show-grid-texture="${this.showGridTextureInSuopView}">
                    </mesh-view>
                </vaadin-split-layout>
            </vaadin-split-layout>
        `;
    }

    static get properties() {
        return {
            modelMeshProvider: {
                type: Object,
                attribute: 'model-mesh-provider'
            },
            suopMeshProvider: {
                type: Object,
                attribute: 'suop-mesh-provider'
            },            
            suopViewportColor: {
                type: String,
                attribute: 'suop-viewport-color'
            },            
            modelViewportColor: {
                type: String,
                attribute: 'model-viewport-color'
            },
            suopViewportColor: {
                type: String,
                attribute: 'suop-viewport-color'
            },
            modelColor: {
                type: String,
                attribute: 'model-color'
            },
            suopColor: {
                type: String,
                attribute: 'suop-color'
            },
            showWireframe: {
                type: Boolean,
                attribute: 'show-wireframe'
            },
            showMeshView: {
                type: Boolean,
                attribute: 'show-mesh-view'
            },
            showSuopView: {
                type: Boolean,
                attribute: 'show-suop-view'
            },
            showSuopGrid: {
                type: Boolean,
                attribute: 'show-suop-grid'
            },
            delta: {
                type: Number,
                attribute: 'delta'
            },
            lambda: {
                type: Number,
                attribute: 'lambda'
            },
            seamlessWeight: {
                type: Number,
                attribute: 'seamless-weight'
            },
            positionWeight: {
                type: Number,
                attribute: 'position-weight'
            },
            splitOrientation: {
                type: String,
                attribute: 'split-orientation'
            },
            solver: {
                type: Boolean,
                attribute: 'solver'
            },
            editingTool: {
                type: String,
                attribute: 'editing-tool'
            },
            meshInteraction: {
                type: String,
                attribute: 'mesh-interaction'
            },
            gridHorizontalColor: {
                type: String,
                attribute: 'grid-horizontal-color'
            },
            gridVerticalColor: {
                type: String,
                attribute: 'grid-vertical-color'
            },
            gridBackgroundColor1: {
                type: String,
                attribute: 'grid-background-color1'
            },
            gridBackgroundColor2: {
                type: String,
                attribute: 'grid-background-color2'
            },
            highlightedFaceColor: {
                type: String,
                attribute: 'highlighted-face-color'
            },
            draggedFaceColor: {
                type: String,
                attribute: 'dragged-face-color'
            },
            fixedFaceColor: {
                type: String,
                attribute: 'fixed-face-color'
            },
            vertexEnergyColor: {
                type: Boolean,
                attribute: 'vertex-energy-color'
            },
            vertexEnergy: {
                type: String,
                attribute: 'vertex-energy'
            },
            gridSize: {
                type: Number,
                attribute: 'grid-size'
            },
            gridTextureSize: {
                type: Number,
                attribute: 'grid-texture-size'
            },
            gridLineWidth: {
                type: Number,
                attribute: 'grid-line-width'
            },
            showOptimizationDataMonitor: {
                type: Boolean,
                attribute: 'show-optimization-data-monitor'
            },
            showUnitGrid: {
                type: Boolean,
                attribute: 'show-unit-grid'
            },
            showGridTextureInSuopView: {
                type: Boolean,
                attribute: 'show-grid-texture-in-suop-view'
            },
            modelFilename: {
                type: String,
                attribute: 'model-filename'
            }
        };
    }

    stateChanged(state) {
        this.splitOrientation = state.splitOrientation;
        this.modelViewportColor = state.modelViewportColor;
        this.suopViewportColor = state.suopViewportColor;
        this.modelColor = state.modelColor;
        this.suopColor = state.suopColor;
        // this._wireframeVisibility = state.wireframeVisibility;
        // this._modelViewVisibility = state.modelViewVisibility;
        // this._suopViewVisibility = state.suopViewVisibility;
        // this._delta = state.delta;
        // this._lambda = state.lambda;
        // this._seamlessWeight = state.seamlessWeight;
        // this._positionWeight = state.positionWeight;
        // this._gridHorizontalColor = state.gridHorizontalColor;
        // this._gridVerticalColor = state.gridVerticalColor;
        // this._gridBackgroundColor1 = state.gridBackgroundColor1;
        // this._gridBackgroundColor2 = state.gridBackgroundColor2;
        // this._highlightedFaceColor = state.highlightedFaceColor;
        // this._draggedFaceColor = state.draggedFaceColor;
        // this._fixedFaceColor = state.fixedFaceColor;
        // this._vertexEnergyColor = state.vertexEnergyColor;
        // this._vertexEnergyType = state.vertexEnergyType;
        // this._gridSize = state.gridSize;
        // this._gridTextureSize = state.gridTextureSize;
        // this._gridLineWidth = state.gridLineWidth;
        // this._unitGridVisibility = state.unitGridVisibility;
        // this._suopViewGridTextureVisibility = state.suopViewGridTextureVisibility;
        // this._optimizationDataMonitorVisibility = state.optimizationDataMonitorVisibility;
        // this._solverState = state.solverState;

        this._reloadModel(state.modelFilename);
    }    

    constructor() {
        super();
        this.modelMeshProvider = new MeshProvider();
        this.suopMeshProvider = new MeshProvider();           
    }

    connectedCallback() {
        super.connectedCallback();
        // this.reloadModelSubscriptionToken = PubSub.subscribe('reload-model', (name, payload) => {
        //     this._reloadModel(payload.modelFilename);
        //     this._modelMeshProvider = new AutoquadsModelMeshProvider(this._engine, this._vertexEnergyType, this._vertexEnergyColor, this._modelColor);
        //     this._suopMeshProvider = new AutoquadsSuopMeshProvider(this._engine, this._vertexEnergyType, this._vertexEnergyColor, this._solverColor);
        // });
            
        // this.reloadModuleSubscriptionToken = PubSub.subscribe('reload-module', () => {
        //     this._reloadModule();
        // });
            
        // this._meshViewFaceDraggingBeginSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging-begin', (name, payload) => {
        //     // this._engine.setMovingTriangleFaceId(payload.face.id);
        //     PubSub.publish('mesh-view-set-dragged-face', payload);
        // });
            
        // this._meshViewFaceDraggingSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging', (name, payload) => {
        //     // this._engine.updateMovingTrianglePosition(payload.offset.x, payload.offset.y);
        // });
            
        // this._meshViewFaceDraggingEndSubscriptionToken = PubSub.subscribe('mesh-view-face-dragging-end', (name, payload) => {
        //     // this._engine.resetMovingTriangleFaceId();
        //     PubSub.publish('mesh-view-reset-dragged-face', payload);
        // });
            
        // this._meshViewFaceSelectedSubscriptionToken = PubSub.subscribe('mesh-view-face-selected', (name, payload) => {
        //     // this._engine.addTriangleToFixedPositionSet(payload.face.id);
        //     PubSub.publish('mesh-view-select-face', payload);
        // });
            
        // this._meshViewFaceUnselectedSubscriptionToken = PubSub.subscribe('mesh-view-face-unselected', (name, payload) => {
        //     // this._engine.removeTriangleFromFixedPositionSet(payload.face.id);
        //     PubSub.publish('mesh-view-unselect-face', payload);
        // });
            
        // this._meshViewFaceHighlightedSubscriptionToken = PubSub.subscribe('mesh-view-face-highlighted', (name, payload) => {
        //     PubSub.publish('mesh-view-highlight-face', payload);
        // });
            
        // this._meshViewFaceUnhighlightedSubscriptionToken = PubSub.subscribe('mesh-view-face-unhighlighted', (name, payload) => {
        //     PubSub.publish('mesh-view-unhighlight-face', payload);
        // });
            
        // this._meshViewVertexDownSubscriptionToken = PubSub.subscribe('mesh-view-vertex-selected', (name, payload) => {
        //     // this._engine.addVertexToIntegerSet(payload.vertexId);
        //     PubSub.publish('mesh-view-select-vertex', payload);
        // });
            
        // this._meshViewVertexDownSubscriptionToken = PubSub.subscribe('mesh-view-vertex-unselected', (name, payload) => {
        //     // this._engine.removeVertexFromIntegerSet(payload.vertexId);
        //     PubSub.publish('mesh-view-unselect-vertex', payload);
        // });
            
        // this.addEventListener('lambda-changed', this._lambdaChanged);
        // this.addEventListener('delta-changed', this._deltaChanged);

        // window.addEventListener('keydown', this._keydown.bind(this));
        // window.addEventListener('keyup', this._keyup.bind(this));

        this._loadModule();
    }
    
    disconnectedCallback() {
        // TODO: Remove event listeners
        super.disconnectedCallback();
    }    

    _reloadModel(modelFilename) {
        if(modelFilename != this._modelFilename) {
            this._modelFilename = modelFilename;
        }

        if(this._modelFilename) {
            this._engine.loadModel(modelFilename);
            this.modelMeshProvider = new AutoquadsModelMeshProvider(this._engine, this._vertexEnergyType, this._vertexEnergyColor, this._modelColor);
            this.suopMeshProvider = new AutoquadsSuopMeshProvider(this._engine, this._vertexEnergyType, this._vertexEnergyColor, this._suopColor);             
        }
    }

    _loadModule() {
        const { join } = require('path');
        let RDSModule = require(join(appRoot, 'node-addon.node'));
        this._engine = new RDSModule.Engine();
        this.modelMeshProvider = new MeshProvider();
        this.suopMeshProvider = new MeshProvider();      
    }

    _reloadModule() {
        this._loadModule();
        this._reloadModel(this._modelFilename);
    }

    _lambdaChanged(lambda) {
        if (!isNaN(lambda)) {
            // this.autoquads.lambda = lambda;
        }
    }

    _deltaChanged(delta) {
        if (!isNaN(delta)) {
            // this.autoquads.delta = delta;
        }
    }

    _integerWeightChanged(integerWeight) {
        if (!isNaN(integerWeight)) {
            // this.autoquads.integerWeight = integerWeight;
        }
    }

    _integerSpacingChanged(integerSpacing) {
        if (!isNaN(integerSpacing)) {
            // this.autoquads.integerSpacing = integerSpacing;
        }
    }

    _seamlessWeightChanged(seamlessWeight) {
        if (!isNaN(seamlessWeight)) {
            // this.autoquads.seamlessWeight = seamlessWeight;
        }
    }

    _positionWeightChanged(positionWeight) {
        if (!isNaN(positionWeight)) {
            // this.autoquads.positionWeight = positionWeight;
        }
    }

    _editingToolChanged(editingTool) {

    }

    _vertexEnergyColorChanged(vertexEnergyColor) {
        this._vertexEnergyColor = new THREE.Color(vertexEnergyColor);
        this._modelMeshProvider.energyColor = this._vertexEnergyColor;
        this._suopMeshProvider.energyColor = this._vertexEnergyColor;
    }

    _vertexEnergyChanged(vertexEnergy) {
        this._modelMeshProvider.vertexEnergy = vertexEnergy;
        this._suopMeshProvider.vertexEnergy = vertexEnergy;
    }

    _modelColorChanged(modelColor) {
        this._modelColor = new THREE.Color(modelColor);
        // this.modelMeshProvider.meshColor = this._modelColor;
    }

    _solverColorChanged(solverColor) {
        this._solverColor = new THREE.Color(solverColor);
        // this.solverMeshProvider.meshColor = this._solverColor;
    }  
}

customElements.define('autoquads-view', AutoquadsView);