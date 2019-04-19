// This file is part of Gratin, a programmable Node-based System
// for GPU-friendly Applications.
//
// Copyright (C) 2013-2014 Romain Vergne <romain.vergne@inria.fr>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef COHERENTSTYLE01_H
#define COHERENTSTYLE01_H

#include <QVBoxLayout>
#include "view/nodeWidget.h"
#include "view/widgetParams.h"
#include "core/nodeHandle.h"
#include "core/nodeTexture2D.h"

// **** a widget containing a keyframed slider ****
class FractalNoisePerlinWidget : public NodeWidget {
 public:
 FractalNoisePerlinWidget(NodeInterface *node) :
  NodeWidget(node),
    _frequency(new FloatSliderWidget(node,"frequency",0,100,8)),
    _style(new FloatSliderWidget(node,"style",0,1,0.4)),
    _amplitude(new FloatSliderWidget(node,"amplitude",0,1,0.8)),
    _nbsamples(new IntSliderWidget(node, "nbSamples", 1, 10, 2)) {
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(_frequency);
    l->addWidget(_style);
    l->addWidget(_amplitude);
    l->addWidget(_nbsamples);
    setLayout(l);
    addChildWidget(_frequency);
    addChildWidget(_style);
    addChildWidget(_amplitude);
    addChildWidget(_nbsamples);
  }

  inline FloatSliderWidget *frequency() const {return _frequency;}
  inline FloatSliderWidget *style() const {return _style;}
  inline FloatSliderWidget *amplitude() const {return _amplitude;}
  inline IntSliderWidget *nbSamples() const {return _nbsamples;}

 private:
  FloatSliderWidget *_amplitude, *_style, *_frequency;
  IntSliderWidget *_nbsamples;
};




// **** the node itself, containing the shader and the widget ****
class FractalNoisePerlinNode : public NodeTexture2D {
 public:
  FractalNoisePerlinNode(PbGraph *parent=NULL,NodeHandle *handle=NULL);
  ~FractalNoisePerlinNode();
  void apply();

  inline NodeWidget  *widget() {return _w;}
  inline virtual void reload() {_pNoise.reload();}
  static QString SHADER_PATH;


 private:


  GPUProgram       _pNoise;
  FractalNoisePerlinWidget *_w;

};




// **** the node handle, containing information displayed in the interface ****
class CoherentStyleHandle : public QObject, public NodeHandleInterface {
  Q_OBJECT
    Q_INTERFACES(NodeHandleInterface)
    Q_PLUGIN_METADATA(IID "Gratin.FractalNoisePerlin")

    public:
  const QString     stringId() const {return "fractalNoisePerlin";}
  unsigned int      version () const {return 1;}

  const QString     name    () const {return "fractalNoisePerlin";}
  const QString     path    () const {return "CustomNoises/";}
  const QString     desc    () const {return "procedural fractal cellular noise";}
  const QString     help    () const {return tr("fractal noise");}

  const QStringList inputNames () const {return QStringList() << "positionWMap" << "depthMap"; }
  const QStringList outputNames() const {return (QStringList() << "rendering");}

  NodeInterface *createInstance(PbGraph *parent) {
    return new FractalNoisePerlinNode(parent,new NodeHandle(this));
  }

  NodeInterface *createInstance(PbGraph *parent,
				const QString &name,
				const QString &path,
				const QString &desc,
				const QString &help,
				const QStringList &inputNames,
				const QStringList &outputNames) {
    return new FractalNoisePerlinNode(parent,new NodeHandle(stringId(),version(),name,path,desc,help,
						   inputNames,outputNames));
  }
};

#endif // COHERENTSTYLE01
